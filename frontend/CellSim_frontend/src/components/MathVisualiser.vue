<template>
	<div v-html="equation"></div>
  </template>
  
  <script>
  export default {
	props: {
	  propEquation: String,
	},
	data() {
	  return {
		equation: ''
	  };
	},
	watch: {
	  propEquation: {
		immediate: true,
		handler(newVal) {
		  this.equation = newVal;
		  // Render MathJax
		  if (window.MathJax) {
			window.MathJax.typesetPromise();
		  }
		}
	  }
	}
  };
  </script>