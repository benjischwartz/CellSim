<template>
<div class=variable 
	:class="{ 'highlighted': isHighlighted }"
	@mouseenter="showMappings" 
	@mouseleave="hideMappings"
	@click="handleClick">
	{{ name }}
</div>
</template>

<script>
import VariableMapping from './VariableMapping.vue';

export default {
	name: 'Variable',
	props: {
		name: String,
		component: String,
		variableMappings: Array,
		highlighted: Array,
	},
	computed: {
		isHighlighted() {
			if (!this.highlighted) return false;
			console.log("This variable is highlighted");
			return this.highlighted.some(mapping=> {
				return (mapping.component === this.component && mapping.variable === this.name);
			});
		},
		coords() {
			return this.getPosition();
		},
	},
	components: {
		VariableMapping
	},
	data() {
		return {
			isHovering: false
		};
	},
	methods: {
		showMappings() {
			this.isHovering = true;
			this.$emit('variable-hover', this.name, this.variableMappings);
		},
		hideMappings() {
			this.isHovering = false;
			this.$emit('variable-hover', null, null);
		},
		handleClick() {
			this.$emit('variable-click', this.name, this.component, this.variableMappings);
		}
	}
}
</script>

<style>
.variable {
  background: lightblue;
  margin: 5px;
  border: 1px solid black;
  border-radius: 25px;
  padding: 0.5em;
}

.variable.highlighted {
	background: greenyellow;
}

.variable:hover {
  background: greenyellow;
}

.arrows line {
  fill: none;
}
	
</style>
