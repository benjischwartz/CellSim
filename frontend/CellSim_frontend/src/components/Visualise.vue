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

  <div class="rectangle" v-for="component in components">
    {{ component.$.name }}
    <div v-if="component.variable">
      <div v-for="variable in component.variable">
         |--- {{ variable.$.name }}
      </div>
   </div>
 </div>

  <div v-for="connection in connections">
    {{ connection.map_components[0].$.component_2 }} -- {{ connection.map_components[0].$.component_1 }} 
 </div>

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
      jsData: "",
      components: [],
      connections: [],
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
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log("js -> %s", str);
      });
      this.visualizeData();
    },

    visualizeData() {
      // creates list of components
      this.components = this.jsData.model.component;
      // creates list of connections
      this.connections = this.jsData.model.connection;
    },
  },
}

</script>

<style>
.rectangle {
  width: 250px;
  height: 200px;
  background-color: lightblue;
  display: flex;
  justify-content: center;
  align-items: center;
  margin: 20px;
}

.connections line {
  stroke: black;
  stroke-width: 2;
}
</style>
