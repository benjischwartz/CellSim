<template>
  <div>
    <!-- Use a <textarea> element for multi-line text input -->
    <textarea v-model="inputValue" @keyup.enter="sendData" class="large-textarea"></textarea>
    <button @click="sendData">Send</button>
  </div>
</template>

<script>
import { sendDataToBackend } from '../services/backendService.js'; 
export default {
  data() {
    return {
      inputValue: "",
    };
  },
  methods: {
    async sendData() {
      // You can access the input value with this.inputValue
      if (this.inputValue.trim() !== "") {
        try {
            const response = await sendDataToBackend({ data: this.inputValue });
            console.log("Data sent to backend:", response);

            // clear input field after successful submission
            this.inputValue = "";
        }
        catch (error) {
            console.error("Error sending data to backend:", error);
        }
      }
    },
  },
};
</script>

<style scoped>
/* Add your component-specific styles here */
.large-textarea {
  width: 100%; /* Adjust the width as needed */
  height: 150px; /* Adjust the height as needed */
  padding: 10px; /* Adjust the padding as needed */
  font-size: 16px; /* Adjust the font size as needed */
  /* Add any other styles you want to customize the textarea */
}
</style>

