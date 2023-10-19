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
  <div id="xml-visualization-container"></div>
</template>

<script>
import axios from 'axios';
import * as d3 from 'd3';
import xml2js from 'xml2js';

export default {
  name: "Visualise",
  data() {
    return {
      file: "",
      resultData: null,
      xmlData: "",
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
      this.xmlData = this.file;
      console.warn("function called", this.file);

      this.visualizeData();
    },
	transformData(xmlObject) {
	  const root = {
	    name: "root",
	    children: [],
	  };
	
	  function transformNode(node) {
	    const newNode = {
	      name: node.$.name || "Unnamed Node",
	      children: [],
	    };
	
	    if (node.variable) {
	      newNode.children = node.variable.map(transformNode);
	    } else if (node.component) {
	      newNode.children = node.component.map(transformNode);
	    }
	
	    return newNode;
	  }
	
	  if (Array.isArray(xmlObject.model)) {
	    root.children = xmlObject.model.map(transformNode);
	  } else if (xmlObject.model) {
        root.children.push(transformNode(xmlObject.model));
      }
	
	  return root;
	},
	
	visualizeData() {
	  if (!this.xmlData) {
	    console.error("No XML data to visualize.");
	    return;
	  }
	
	  xml2js.parseString(this.xmlData, (err, result) => {
	    if (!err) {
	      const treeData = this.transformData(result);

          // calculate tree dimensions
      	  const treeLayout = d3.tree().size([1200, 800]); // Adjust these values as needed
      	  const treeRoot = d3.hierarchy(treeData);
      	  const treeLayoutData = treeLayout(treeRoot);

		  // Calculate the required SVG container dimensions
      	  const treeWidth = treeLayoutData.descendants().length * 200; // Adjust as needed
      	  const treeHeight = treeLayoutData.height * 200; // Adjust as needed
	
	      d3.select("#xml-visualization-container").html("");
	
	      const svg = d3
	        .select("#xml-visualization-container")
	        .append("svg")
	        .attr("width", treeWidth)
	        .attr("height", treeHeight);
	
	      const links = svg
	        .selectAll(".link")
	        .data(treeLayoutData.links())
	        .enter()
	        .append("path")
	        .attr("class", "link")
	        .attr("d", (d) => {
                return `M${d.source.x},${d.source.y} L${d.target.x},${d.target.y}`
            });
	
	      const nodes = svg
	        .selectAll(".node")
	        .data(treeLayoutData.descendants())
	        .enter()
	        .append("g")
	        .attr("class", "node")
	        .attr("transform", (d) => `translate(${d.x},${d.y})`);
	
	      nodes.append("circle").attr("r", 10);
	
	      nodes
	        .append("text")
	        .attr("dy", ".35em")
	        .attr("y", (d) => (d.children ? -20 : 20))
	        .style("text-anchor", "middle")
	        .text((d) => d.data.name);
	    }
	  });
	},
  },
};
</script>

