<template>
  <!--<div>
    <h1>json data from backend</h1>
    <div v-if="loading">Loading...</div>
    <pre v-else>{{ jsonData }}</pre>
  </div> <br /><br /> -->
    <h1>POST Method API</h1>
    <input type="text" name="file" v-model="file" /> <br /><br />
    <button v-on:click="sendData()">Send file data</button> <br /><br />
</template>

<script>
import axios from 'axios'
export default {
  name: "App",
  data()
  {
    return {
      file: "",
    };
  },

  methods:{
    async sendData()
    {
      let result = await axios.post("http://localhost:8000/api/validate", {
        file: this.file
      })
      console.warn("function called", this.file)
    },
  },
};
</script>

