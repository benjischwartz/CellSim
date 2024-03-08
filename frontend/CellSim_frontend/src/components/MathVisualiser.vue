<template>
	<div>
		<textarea v-model="formula" cols="30" rows="10"></textarea>
  	<vue-mathjax :formula="propEquation"></vue-mathjax>
	</div>
</template>
	
<script>
import VueMathjax from 'vue-mathjax-next'

export default {
	name: 'MathVisualizer',
	props: {
		propEquation: String,
	},
	components: {
		'vue-mathjax': VueMathjax
	}
}
</script>
	