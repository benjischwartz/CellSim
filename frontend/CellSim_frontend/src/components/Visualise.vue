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

  <div class="container">
    <div class="component" v-for="component in components">
      {{ component.$.name }}
      <div v-if="component.variable">
        <div class="variable" v-for="variable in component.variable">
           {{ variable.$.name }}
        </div>
     </div>
    </div>
  </div>
  <br /><br />

  <h2> Connections List </h2>
  <br /><br />
  <div v-for="connection in connections">
    {{ connection.map_components[0].$.component_2 }} -- {{ connection.map_components[0].$.component_1 }} 
 </div>

  <h2> Groups List </h2>
  <br /><br />
  <div v-for="group in groups">
    <!-- get containment relationships -->
    {{ group.relationship_ref[0].$.relationship}}
   </div>

  <!-- create tree-like structure of group data -->
  <h2> Tree </h2>
  <br /><br />
  <div>
    <TreeContainer :propData="treeData"/>
  </div>

</template>

<script>
import TreeContainer from './TreeContainer.vue'
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
      groups: [],
      treeData: {
        name: "Root"
      }
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
      // creates list of groups
      this.groups = this.jsData.model.group;
      
    },
  },
  components: {
    TreeContainer
  },
}

</script>

<style>
.container {
  display: flex;
  background: antiquewhite;
  flex-wrap: wrap;
  border-radius: 25px;
}

.component {
  flex: 1;  
  width: 250px;
  margin: 1em;
}

.variable {
  background: lightblue;
  margin: .5em;
}

.container > div {
  font-size: 1vw;
  padding: .5em;
  background: silver;
  border: 1px solid black;
  border-radius: 25px;
}

</style>
