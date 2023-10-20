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
  name: "visualise",
  data() {
    return {
      file: "",
      resultData: null,
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

      this.visualizeData();
    },
    visualizeData() {
      if (!this.file) {
        console.error("No XML data to visualize.");
        return;
      }
      // xml data stored in this.file
      // use xml2json to transform to javascript object
      console.log("original data -> %s", this.file);
      xml2js.parseString(this.file, (err, result) => {
        if (err) {
          throw err;
        }
          const jsData = result;
          const str = JSON.stringify(jsData, null, 2);
          console.log("js -> %s", str);
      });
    },
  },
};
</script>

