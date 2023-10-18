<template> 
  <div>
    <h1>Visualise CellML File</h1>
    <!-- Add input for file upload -->
	<input type="file" @change="handleFileUpload" accept=".xml" />
    <br /><br />
    <!-- Add text boxes for model name and model id -->
    <textarea name="file" v-model="file" placeholder="Or paste File Data Here"></textarea> <br /><br />
    <button @click="sendData()">Visualise</button> <br /><br />
  </div>

    <!-- Display the result data in a panel -->
    <br /><br />
    <div v-if="resultData">
      <h2>Result Data</h2>
      <pre>{{ resultData.data}}</pre>
    </div>
    <br /><br />
    <!-- Add an SVG container for visualization -->
	<div id="xml-visualization-container"></div>

      <!-- Add an iframe to embed the Tidy Tree visualization from 
		Observable with XML data in URL parameter -->
    <iframe
      width="800"
      height="600"
      :src="yourObservableEmbedURL"
      frameborder="0"
    ></iframe>

</template>

<script>
import axios from 'axios'
import * as d3 from 'd3'
export default {
  name: "Validate",
  data()
  {
    return {
      file: "",
      model_name: "",
      model_id: "",
      resultData: null, // Added data property to store result
	  yourObservableEmbedURL: "",
    };
  },

  methods:{
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
    async sendData()
    {
      let result = await axios.post("http://localhost:8000/api/visualise", {
        file: this.file
      })
      this.resultData = result;
      console.warn("function called", this.file)

	  this.renderXMLData(this.file);
	  this.yourObservableEmbedURL = "https://observablehq.com/d/5dfc029938a00648?xmlData=${encodeURIComponent(this.file)}";
    },

	renderXMLData(xmlData) {
      console.log(xmlData);
	  const svg = d3.select("#xml-visualization-container").append("svg")
		.attr("width", 600)
		.attr("height", 400);

	  // Parse the XML data
	  const parser = new DOMParser();
	  const xmlDoc = parser.parseFromString(xmlData, "text/xml");

	  // Create a hierarchical layout for the XML tree
	  const treeLayout = d3.tree().size([400, 300]);
	  const treeData = d3.hierarchy(this.xmlToD3Hierarchy(xmlDoc.documentElement));

	  // Create a group element to hold the tree elements
	  const treeGroup = svg.append("g")
		.attr("transform", "translate(100, 50)");

	  // Create tree links (edges)
	  const links = treeGroup.selectAll(".link")
		.data(treeLayout(treeData).links())
		.enter().append("path")
		.attr("class", "link")
		.attr("d", d => {
		  return `M${d.source.x},${d.source.y}C${d.source.x},${(d.source.y + d.target.y) / 2} ${d.target.x},${(d.source.y + d.target.y) / 2} ${d.target.x},${d.target.y}`;
		});

	  // Create tree nodes (circles)
	  const nodes = treeGroup.selectAll(".node")
		.data(treeData.descendants())
		.enter().append("g")
		.attr("class", "node")
		.attr("transform", d => `translate(${d.x},${d.y})`);

	  nodes.append("circle")
		.attr("r", 10);

	  nodes.append("text")
		.attr("dy", ".35em")
		.attr("y", d => (d.children ? -20 : 20))
		.style("text-anchor", "middle")
		.text(d => d.data.nodeName);

	  function collapse(d) {
		if (d.children) {
		  d._children = d.children;
		  d._children.forEach(collapse);
		  d.children = null;
		}
	  }

	  treeData.children.forEach(collapse);
	},
	xmlToD3Hierarchy(node) {
	  const data = {
		nodeName: node.nodeName,
	  };

	  if (node.nodeType === 3) { // Text nodes
		data.nodeValue = node.nodeValue.trim();
	  } else if (node.nodeType === 1 && node.childNodes.length > 0) { // Element nodes with children
		data.children = Array.from(node.childNodes)
		  .filter(childNode => childNode.nodeType === 1 || childNode.nodeType === 3)
		  .map(childNode => this.xmlToD3Hierarchy(childNode));
	  }

	  return data;
	},

  },
};
</script>
