<template> 
  <div>
    <h1>Visualise CellML File</h1>
    <!-- Add text boxes for model name and model id -->
    <textarea name="file" v-model="file" placeholder="Paste File Data Here"></textarea> <br /><br />
    <button v-on:click="sendData()">Visualise</button> <br /><br />
  </div>

    <!-- Display the result data in a panel -->
    <br /><br />
    <div v-if="resultData">
      <h2>Result Data</h2>
      <pre>{{ resultData.data}}</pre>
    </div>

</template>

<script>
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
      let result = await axios.post("http://localhost:8000/api/visualise", {
        file: this.file
      })
      this.resultData = result;
      console.warn("function called", this.file)
    },
  },
};
</script>
