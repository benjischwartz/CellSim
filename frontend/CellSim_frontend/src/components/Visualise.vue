<template>
<div>
    <h1>Visualize CellML File</h1>
    <!-- Add input for file upload -->
    <input type="file" @change="handleFileUpload" accept=".xml" />
    <br /><br />
    <!-- Add text boxes for model name and model id -->
    <textarea name="file" v-model="file" placeholder="Or paste File Data Here"></textarea> 
    <br /><br />
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
    <div class="{ 'component': true, 'collapsed': isCollapsed }" v-for="component in components">
        <div class="name">{{ component.$.name }} </div>
        <button class="collapse-button" @click="toggleCollapse(component.$.name)">{{ isCollapsed[component.$.name] ? 'Expand' : 'Collapse' }}</button>
        <div v-if="component.variable && !isCollapsed[component.$.name]">
            <div v-for="variable in component.variable">
                <Variable :name=variable.$.name :variableMappings="getMappingsForVariable(variable.$.name, component.$.name)"/>
            </div>
        </div>
    </div>
</div>

<br /><br />

<h2> Connections List </h2>
<div v-for="connection in connections">
    {{ connection.map_components[0].$.component_2 }} -- {{ connection.map_components[0].$.component_1 }}
</div>

<br /><br />

<h2> Tree Groups List </h2>
<br /><br />
<div class= "group-wrapper">
    <div v-for="group in groups">
        <!-- get containment relationships -->
        <h3> {{ group.relationship_ref[0].$.relationship}} {{ "relationship" }} </h3>
        <div>
            <TreeContainer :propData=group.component_ref[0] :components=components :connections=connections />
        </div>
    </div>
</div>

</template>

<script>
import TreeContainer from './TreeContainer.vue';
import axios from 'axios';
import * as d3 from 'd3';
import xml2js from 'xml2js';
import Variable from './Variable.vue';
import VariableMapping from "./VariableMapping.vue"

export default {
  name: 'visualise',
  data() {
    return {
      file: '',
      resultData: null,
      jsData: '',
      components: [],
      connections: [],
      groups: [],
      isCollapsed: {},
    };
  },
  watch: {
    components: {
        handler(newVal) {
            this.initializeIsCollapsed();

        },
        immediate: true,
    },
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
      const result = await axios.post('http://localhost:8000/api/visualise', {
        file: this.file,
      });
      this.resultData = result;
      console.warn('function called', this.file);
      xml2js.parseString(this.file, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
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
    getMappingsForVariable(variableName, componentName) {
        let mappings = [];
        for (let connection of this.connections) {
            if (connection.map_components[0].$.component_1 == componentName) {
                for (let mapping of connection.map_variables) {
                    if (mapping.$.variable_1 == variableName) {
                        mappings.push({component: connection.map_components[0].$.component_2,
                                    variable: mapping.$.variable_2});
                    }
                }
            } else if (connection.map_components[0].$.component_2 == componentName) {
                for (let mapping of connection.map_variables) {
                    if (mapping.$.variable_2 == variableName) {
                        mappings.push({component: connection.map_components[0].$.component_1,
                                    variable: mapping.$.variable_1});
                    }
                }
            }
        }
        return mappings;
    },
    initializeIsCollapsed() {
        this.isCollapsed = {};
        for (const component of this.components) {
            this.isCollapsed[component.$.name] = false;
        }
    },
    toggleCollapse(componentName) {
        this.isCollapsed[componentName] = !this.isCollapsed[componentName];
    }
  },
  components: {
    TreeContainer,
    Variable,
    VariableMapping,
  },
};

</script>

<style>

.container {
  display: flex;
  background: antiquewhite; 
  margin: 1em;
  justify-content: center;
  padding: 1em;
  border-radius: 25px;
  gap: 10px 20px;
}

.group-wrapper {
  display: flex;
  border-radius: 25px;
  flex-direction: column;
  gap: 10px 20px;
}

.component {
  position: relative;
  flex: 1;
  flex-wrap: wrap;
  flex-direction: row;
  display: flex;
}

.variable {
  background: lightblue;
  border: 1px solid black;
  border-radius: 25px;
}

.name {
    padding: 10px 0;
}

.collapse-button {
    top: 10px;
    right: 10px;
}

.collapsed {
    height: fit-content;
}

</style>
