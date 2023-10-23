<template>
  <div>
    <h1>Visualize CellML File</h1>
    <!-- Add input for file upload -->
    <input type="file" @change="handleFileUpload" accept=".xml" />
    <br /><br />
    <!-- Add text boxes for model name and model id -->
    <textarea name="file" v-model="file" placeholder="Or paste File Data Here"></textarea> <br /><br />
    <button @click="sendData()">Visualize</button> <br /><br />
  </div>

  <!-- Display the result data in a panel -->
  <br /><br />
  <div v-if="resultData">
    <h2>Result Data</h2>
    <pre>{{ resultData.data }}</pre>
  </div>
  <br /><br />

    
  <!-- Add an SVG container for visualization -->
  <div id="svg-container"></div>

</template>

<script>
import axios from 'axios';
import * as d3 from 'd3';
import xml2js from 'xml2js';

export default {
  name: "visualise",
  data() {
    return {
      file: "",
      resultData: null,
      treeData: "",
    };
  },
  methods: {
    handleFileUpload(event) {
      const selectedFile = event.target.files[0];
      if (selectedFile) {
        const reader = new FileReader();
        reader.onload = (e) => {
          this.file = e.target.result;
        };
        reader.readAsText(selectedFile);
      }
    },
    async sendData() {
      let result = await axios.post("http://localhost:8000/api/visualise", {
        file: this.file
      });
      this.resultData = result;
      console.warn("function called", this.file);
      xml2js.parseString(this.file, (err, result) => {
        if (err) {
          throw err;
        }
        const jsData = result;
        const str = JSON.stringify(jsData, null, 2);
        console.log("js -> %s", str);
        this.treeData = jsData;
      });
      this.visualizeData();
    },

    visualizeData() {
      const dx = 12;
      const dy = 120;
      var tree = d3.tree().nodeSize([dx, dy]);
      var treeLink = d3.linkHorizontal().x(d => d.y).y(d => d.x);
      function graph(root, {
        label = d => d.data.id, 
        highlight = () => false,
        marginLeft = 40
      } = {}) {
        root = tree(root);

        let x0 = Infinity;
        let x1 = -x0;
        root.each(d => {
          if (d.x > x1) x1 = d.x;
          if (d.x < x0) x0 = d.x;
        });

        const treeWidth = x1 - x0 + dx * 2;
        const treeHeight = root.height * dy;

        // Adjust the viewBox and container size based on the tree dimensions
        const width = treeWidth + marginLeft;
        const height = treeHeight + dx;

        const svg = d3.create("svg")
            .attr("viewBox", [0, 0, width, height])
            .style("overflow", "visible");
        
        const g = svg.append("g")
            .attr("font-family", "sans-serif")
            .attr("font-size", 10)
            .attr("transform", `translate(${marginLeft},${dx})`);
          
        const link = g.append("g")
          .attr("fill", "none")
          .attr("stroke", "#555")
          .attr("stroke-opacity", 0.4)
          .attr("stroke-width", 1.5)
        .selectAll("path")
          .data(root.links())
          .join("path")
            .attr("stroke", d => highlight(d.source) && highlight(d.target) ? "red" : null)
            .attr("stroke-opacity", d => highlight(d.source) && highlight(d.target) ? 1 : null)
            .attr("d", treeLink);
        
        const node = g.append("g")
            .attr("stroke-linejoin", "round")
            .attr("stroke-width", 3)
          .selectAll("g")
          .data(root.descendants())
          .join("g")
            .attr("transform", d => `translate(${d.y},${d.x})`);

        node.append("circle")
            .attr("fill", d => highlight(d) ? "red" : d.children ? "#555" : "#999")
            .attr("r", 2.5);

        node.append("text")
            .attr("fill", d => highlight(d) ? "red" : null)
            .attr("stroke", "white")
            .attr("paint-order", "stroke")
            .attr("dy", "0.31em")
            .attr("x", d => d.children ? -6 : 6)
            .attr("text-anchor", d => d.children ? "end" : "start")
            .text(label);
        
        // Append the generated SVG to the #svg-container
        d3.select("#svg-container").node().appendChild(svg.node());
      }
    var family = d3.hierarchy({
      name: "root",
      children: [
        {name: "child #1"},
        {
          name: "child #2",
          children: [
            {name: "grandchild #1"},
            {name: "grandchild #2"},
            {name: "grandchild #3"}
          ]
        }
      ]
    })

      graph(family, { label: d => d.data.name });
    },
  },
}

</script>
