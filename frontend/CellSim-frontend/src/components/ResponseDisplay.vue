<template>
  <div class="response-display">
    <h2>Response from Server</h2>
    <pre>{{ responseData }}</pre>
  </div>
</template>

<script>
export default {
  props: {
    responseData: String, // Prop to receive the response data
  },
};
</script>

<style scoped>
/* Add styling for your response display component */
.response-display {
  border: 1px solid #ccc;
  padding: 16px;
  margin: 16px;
}
</style>

