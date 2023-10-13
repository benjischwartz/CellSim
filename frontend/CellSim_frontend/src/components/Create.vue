<template> 
  <div>
    <h1>Create CellML File</h1>
    <!-- Add text boxes for model name and model id -->
    <input type="text" name="model_name" v-model="model_name" placeholder="Model Name" />
    <input type="text" name="model_id" v-model="model_id" placeholder="Model ID" />
    <button v-on:click="sendModel()">Upload</button> <br /><br />
  </div> <br /><br />

    <!-- Display the result data in a panel -->
    <div v-if="resultData">
      <h2>Result Data</h2>
      <pre>{{ resultData.data }}</pre>
      <input type="text" name="units_name" v-model="units_name" placeholder="Units Name" />
      <button v-on:click="addUnits()">Add</button><br /><br />
      <input type="text" name="component_name" v-model="component_name" placeholder="Component Name" />
      <button v-on:click="addComponent()">Add</button><br /><br />
    </div>

	  <!-- Add the "Visualize" toggle -->
	  <label>
	    Visualize
	    <input type="checkbox" v-model="visualise" @click="toggleVisualise()" />
	  </label>

	  <!-- Add a visualization element (e.g., a div) that is conditionally displayed -->
	  <div v-if="visualise">
	    <!-- Add your visualization content here -->
	  </div>

</template>

<script>
import axios from 'axios'
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
      resultData: null, // data property to store result
    };
  },

  methods:{
    async sendModel()
    {
      let result = await axios.post("http://localhost:8000/api/create", {
        model_name: this.model_name,
        model_id: this.model_id,
        visualise: this.visualise
      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
    async addUnits()
    {
      let result = await axios.post("http://localhost:8000/api/edit", {
        edit_type: "add_units",
        units_name: this.units_name,
        file: this.resultData.data,
        model_name: this.model_name,
        model_id: this.model_id,
        visualise: this.visualise
      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
    async addComponent()
    {
      let result = await axios.post("http://localhost:8000/api/edit", {
        edit_type: "add_component",
        component_name: this.component_name,
        file: this.resultData.data,
        model_name: this.model_name,
        model_id: this.model_id,
        visualise: this.visualise
      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
    async toggleVisualise()
    {
      let result = await axios.post("http://localhost:8000/api/togglelocal", {
        visualise: !(this.visualise)
      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
  },
};
</script>
