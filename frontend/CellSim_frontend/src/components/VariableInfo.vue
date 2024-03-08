<template>
<div class="variable-info">
	<span>Variable: {{ propVariableName }}</span>
	<div>
		<input v-model="updateVariableName" type="text" placeholder="Update Variable Name...">
		<button @click="updateVariableName">Update</button>
	</div>
	<hr>
	<span>Component: {{ propComponentName }}</span>
	<hr>
	<div>Variable Mappings: 
		<div v-for="mapping in propVariableMappings">
			{{ propComponentName }} === {{ mapping.component }}
		</div>
	</div>
	<hr>
	<span>Units: {{ propVariableUnits }}</span>
	<hr>
	<div>
		<MathVisualiser :propEquation=this.propEquation></MathVisualiser>
	</div>
</div>
</template>
  
<script>
import MathVisualiser from './MathVisualiser.vue';

export default {
	name: 'VariableInfo',
	props: {
		propVariableName: String,
		propComponentName: String,
		propVariableMappings: Object,
		propVariableUnits: String,
		propEquation: String,
	},
	data () {
		return {
			updateVariableName: '',
		}
	},
	methods: {
		updateVariableName() {
			this.emit$('update-variable-name', this.updateVariableName);
		}
	},
	components: {
		MathVisualiser,
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
	width: 300px;
  background: lightsalmon;
}

.variable-info hr {
	width: 100%;
}
</style>
  