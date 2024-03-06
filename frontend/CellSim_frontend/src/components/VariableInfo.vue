<template>
<div class="variable-info">
	<div>Variable: {{ propVariableInfo[0].variable }}</div>
	<hr>
	<div>Parent Component: {{ propVariableInfo[1] }}</div>
	<hr>
	<div>Variable Mappings: {{  propVariableInfo[2] }}</div>
</div>
</template>
  
<script>
export default {
	name: 'VariableInfo',
	props: {
		propVariableInfo: Object,
	}
}
</script>

<style>
.variable-info {
	display: flex;
	flex-flow: wrap;
	position: fixed;
	top: 0;
	right: 0;
	border: 1px solid black;
  border-radius: 25px;
  padding: 0.5em;
  margin: 5px;
  background: lightsalmon;
}

.variable-info hr {
	width: 100%;
}
</style>
  