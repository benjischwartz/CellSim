<template>
<div class="variable-info">
	<span><b>Variable:</b> {{ propVariableName }}</span>
	<hr>
	<span><b>Component:</b> {{ propComponentName }}</span>
	<hr>
	<div><b>Variable Mappings:</b> 
		<div v-for="mapping in propVariableMappings">
			{{ propComponentName }} === {{ mapping.component }}
		</div>
	</div>
	<hr>
	<span><b>Units: </b>{{ propVariableUnits }}</span>
	<hr>
	<div>
	<span> <b>Equation:</b> </span>
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
	width: 500px;
  background: beige;
}

.variable-info hr {
	width: 100%;
}
</style>
  