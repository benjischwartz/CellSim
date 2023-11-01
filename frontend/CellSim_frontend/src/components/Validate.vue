<template> 
  <div>
    <h1>Validate CellML File</h1>
    <!-- Add text boxes for model name and model id -->
    <textarea name="file" v-model="file" placeholder="Paste File Data Here"></textarea> <br /><br />
    <button v-on:click="sendData()">Validate</button> <br /><br />
  </div>

    <!-- Display the result data in a panel -->
    <br /><br />
    <div v-if="resultData">
      <h2>Result Data</h2>
      <pre>{{ resultData.data }}</pre>
    </div>

</template>

<script>
import TreeContainer from './TreeContainer.vue'
import axios from 'axios'
export default {
  name: "Validate",
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
