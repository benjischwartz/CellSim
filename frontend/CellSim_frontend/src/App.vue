<template> <!--<div>
    <h1>json data from backend</h1>
    <div v-if="loading">Loading...</div>
    <pre v-else>{{ jsonData }}</pre>
  </div> <br /><br /> -->
  <div>
    <h1>POST Method API</h1>
    <textarea name="file" v-model="file"></textarea> <br /><br />
    <button v-on:click="sendData()">Send file data</button> <br /><br />
  </div>

    <!-- Display the result data in a panel -->
    <div v-if="resultData">
      <h2>Result Data</h2>
      <pre>{{ resultData }}</pre>
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
  },
};
</script>

