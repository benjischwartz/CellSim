<template> 
  <div>
    <h1>Create CellML File</h1>
    <!-- Add text boxes for model name and model id -->
    <input type="text" name="model_name" v-model="model_name" placeholder="Model Name" />
    <input type="text" name="model_id" v-model="model_id" placeholder="Model ID" />
    <button v-on:click="sendModel()">Upload</button> <br /><br />
  </div>

    <!-- Display the result data in a panel -->
    <div v-if="resultData">
      <h2>Result Data</h2>
      <pre>{{ resultData.data }}</pre>
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
      resultData: null, // Added data property to store result
    };
  },

  methods:{
    async sendData()
    {
      let result = await axios.post("http://localhost:8000/api/validate", {
        file: this.file
      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
    async sendModel()
    {
      let result = await axios.post("http://localhost:8000/api/create", {
        model_name: this.model_name,
        model_id: this.model_id
      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
  },
};
</script>
