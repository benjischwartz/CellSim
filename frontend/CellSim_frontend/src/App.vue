<template> <!--<div>
    <h1>json data from backend</h1>
    <div v-if="loading">Loading...</div>
    <pre v-else>{{ jsonData }}</pre>
  </div> <br /><br /> -->
  <div>
    <h1>CellSim API</h1>
    <textarea name="file" v-model="file" placeholder="Paste File Data Here"></textarea> <br /><br />
    <button v-on:click="sendData()">Validate</button> <br /><br />
  </div> <br /><br />

    <!-- Add text boxes for model name and model id -->
    <input type="text" name="model_name" v-model="model_name" placeholder="Model Name" />
    <input type="text" name="model_id" v-model="model_id" placeholder="Model ID" />
    <button v-on:click="sendModel()">Upload</button> <br /><br />

    <!-- Display the result data in a panel -->
    <div v-if="resultData">
      <h2>Result Data</h2>
      <pre>{{ resultData.data }}</pre>
    </div>

</template>

<script>
import axios from 'axios'
export default {
  name: "App",
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
        // TODO: Extract data field from JSON string

        //      let resultObj = JSON.parse(result, (key, value) => {
        //        if (key === "data") {
        //          this.resultData = result;
        //        }
        //      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
    async sendModel()
    {
      let result = await axios.post("http://localhost:8000/api/create", {
        model_name: this.model_name,
        model_id: this.model_id
      })
        // TODO: Extract data field from JSON string

        //      let resultObj = JSON.parse(result, (key, value) => {
        //        if (key === "data") {
        //          this.resultData = result;
        //        }
        //      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
  },
};
</script>

