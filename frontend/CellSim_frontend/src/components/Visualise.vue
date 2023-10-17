<template> 
  <div>
    <h1>Visualise CellML File</h1>
    <!-- Add input for file upload -->
	<input type="file" @change="handleFileUpload" accept=".xml" />
    <br /><br />
    <!-- Add text boxes for model name and model id -->
    <textarea name="file" v-model="file" placeholder="Or paste File Data Here"></textarea> <br /><br />
    <button @click="sendData()">Visualise</button> <br /><br />
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
