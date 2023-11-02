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
      <h2>XML Data</h2>
      <pre>{{ xmlData }}</pre>
      <input type="text" name="units_name" v-model="units_name" placeholder="Units Name" />
      <button v-on:click="addUnits()">Add</button><br /><br />
      <input type="text" name="component_name" v-model="component_name" placeholder="Component Name" />
      <button v-on:click="addComponent()">Add</button><br /><br />
    </div>

      <br /><br />
	  <!-- Add the "Visualize" toggle -->
	  <label>
	    Visualize
	    <input type="checkbox" v-model="visualise" @click="createComponentList()"/>
	  </label>
      <br /><br />

	  <!-- Add a visualization element (e.g., a div) that is conditionally displayed -->
	  <div v-if="visualise">
        <!-- display formatted data-->
          <h2>Formatted Data</h2>
          <pre>{{ formattedData }}</pre>
          <br /><br />

	  <!-- Add the "Visualize" toggle -->
        <!-- Simple container view-->
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

        <!-- commenting out until groups can be added
          <h2> Tree Groups List </h2>
          <br /><br />
          <div class="container">
            <div v-for="group in groups">
              <h3>
                {{ group.relationship_ref[0].$.relationship}} {{ "relationship" }}
              </h3>
              <TreeContainer :propData=group.component_ref[0] :components=components />
              <br /><br />
             </div>
            </div>
        -->


	  </div>

</template>

<script>
import TreeContainer from './TreeContainer.vue';
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
      components: [],
      groups: [],
    };
  },

  methods:{
    async sendModel()
    {
      let result = await axios.post("http://localhost:8000/api/create", {
        model_name: this.model_name,
        model_id: this.model_id,
      })

      // Separate the response data into XML and formatted data
      const responseText = result.data;
      this.xmlData = responseText.substring(0, responseText.indexOf(separator));
      this.formattedData = responseText.substring(this.xmlData.length + separator.length);
      createComponentList();

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
      createComponentList();
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
      this.xmlData = responseText.substring(0, responseText.indexOf(separator));
      this.formattedData = responseText.substring(this.xmlData.length + separator.length);
      createComponentList();
    },
    async createComponentList()
    {
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.components = result.model.component;
        this.groups = result.model.group;
      });
        
    },

  },
  components: {
    TreeContainer
  },
};
</script>

<style>
.container {
  display: flex;
  width: 600px;
  background: antiquewhite;
  margin: 10em;
  flex-wrap: wrap;
  justify-content: center;
  align-content: stretch;
  padding: 2em;
  border-radius: 25px;
  gap: 10px 20px;
}

.component {
  flex: 1;  
  width: 500px;
  padding: .5em;
  margin: 1em;
}

.variable {
  background: lightblue;
  margin: .5em;
  border: 1px solid black;
  border-radius: 25px;
}

.container > div {
  font-size: 1vw;
  padding: .5em;
  background: silver;
  flex-shrink: 1;
  border: 1px solid black;
  border-radius: 25px;
}

</style>
