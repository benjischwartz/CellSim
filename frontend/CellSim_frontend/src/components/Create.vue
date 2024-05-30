<template> 
  <div>
    <h1>Create CellML File</h1>
    <!-- Add text boxes for model name and model id -->
    <input type="text" name="model_name" v-model="model_name" placeholder="Model Name" />
    <input type="text" name="model_id" v-model="model_id" placeholder="Model ID" />
    <button v-on:click="sendModel()">Upload</button> <br /><br />
  </div> <br /><br />

  <!-- Display the result data in a panel -->
  <div v-if="xmlData">
    <h2> {{ model_name }}</h2>
    <pre>{{ formattedData }}</pre>
    <div v-if="jsData.model.component">
    <h2>Component View</h2>
    <ComponentView 
      :propData=jsData
      @add-component="addComponent"
      @add-nested-component="addNestedComponent"
      @add-variable="addVariable"
    /> </div>
    <button>Add Metadata</button> <br />
    <button @click="showUnitsInput=true">Add Units</button> <br />
    <div v-if="showUnitsInput">
      <input type="text" name="units_name" v-model="units_name" placeholder="Units Name" />
      <button v-on:click="addUnits()">Add</button><br /><br />
    </div>
    <button @click="showComponentInput=true">Add Component</button> <br />
    <div v-if="showComponentInput">
      <input type="text" name="component_name" v-model="component_name" placeholder="Component Name" />
      <button v-on:click="addComponent()">Add</button><br /><br />
    </div>
    <button>Set Model Name </button> <br />
    <button>Export </button> <br />
  </div>


</template>

<script>
import ComponentView from './ComponentView.vue'
import axios from 'axios';
import xml2js from 'xml2js';

let separator = "*separator*";
export default {
  name: "Create",
  data()
  {
    return {
      file: "",
      model_name: "",
      model_id: "",
      units_name: "",
      component_name: "",
      edit_type: "",
      visualise: false,
      formattedData: "", // data property to store the formatted result
      xmlData: "", // data property to store xml result
      jsData: "",
      components: [],
      items: [],
      groups: [],
      showComponentInput: false,
      showUnitsInput: false,
    };
  },

  methods:{
    async sendModel()
    {
      let result = await axios.post("http://localhost:8000/api/create", {
        model_name: this.model_name,
        model_id: this.model_id,
      })
      console.log("sending model");

      // Separate the response data into XML and formatted data
      const responseText = result.data;
      this.xmlData = responseText.substring(0, responseText.indexOf(separator));
      this.formattedData = responseText.substring(this.xmlData.length + separator.length);
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
    },
    async addUnits()
    {
      let result = await axios.post("http://localhost:8000/api/edit", {
        edit_type: "add_units",
        units_name: this.units_name,
        file: this.xmlData,
        model_name: this.model_name,
        model_id: this.model_id,
      })
      // Separate the response data into XML and formatted data
      const responseText = result.data;
      this.xmlData = responseText.substring(0, responseText.indexOf(separator));
      this.formattedData = responseText.substring(this.xmlData.length + separator.length);
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
      this.showUnitsInput = false;
    },
    async addComponent()
    {
      let result = await axios.post("http://localhost:8000/api/edit", {
        edit_type: "add_component",
        component_name: this.component_name,
        file: this.xmlData,
        model_name: this.model_name,
        model_id: this.model_id,
      })
        
      // Separate the response data into XML and formatted data
      const responseText = result.data;
      console.log(responseText);
      this.xmlData = responseText.substring(0, responseText.indexOf(separator));
      this.formattedData = responseText.substring(this.xmlData.length + separator.length);
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
      this.showComponentInput = false;
    },
  },
  async addNestedComponent(componentName, parentComponent)
		{
			console.log("Adding component!!!")
			try {
				const result = await axios.post('http://localhost:8000/api/edit', {
					edit_type: "add_child_component",
					parent_component_name: parentComponent,
					child_component_name: componentName,
					file: this.resultData,
				});
				console.log(result)
				this.resultData = result.data;
				this.file = result.data;
				xml2js.parseString(result.data, (err, result) => {
					if (err) {
						throw err;
					}
					this.jsData = result;
					const str = JSON.stringify(result, null, 2);
					console.log('js -> %s', str);
				});
			} catch (error) {
				console.error('An error occurred:', error);
				this.errorData = error.response.data
				this.jsData = null
			}
		},

  components: {
    ComponentView,
  },
};
</script>