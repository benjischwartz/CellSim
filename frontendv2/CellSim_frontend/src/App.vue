<template>
  <div>
    <h1>JSON Data from Backend</h1>
    <div v-if="loading">Loading...</div>
    <pre v-else>{{ jsonData }}</pre>
  </div>
</template>

<script>
export default {
  data() {
    return {
      jsonData: null,
      loading: true,
    };
  },
  async created() {
    try {
      const response = await fetch('http://localhost:8000'); // Replace with the actual API URL
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }
      const data = await response.json();
      this.jsonData = JSON.stringify(data, null, 2);
    } catch (error) {
      console.error('Error fetching data:', error);
    } finally {
      this.loading = false;
    }
  },
};
</script>

