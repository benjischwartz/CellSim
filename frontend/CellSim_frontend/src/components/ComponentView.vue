<template>
<div class="container">
	<div class="{ 'component': true, 'collapsed': isCollapsed }" v-for="component in components">
		<div class="name">{{ component.$.name }} </div>
		<button 
			class="collapse-button" 
			@click="toggleCollapse(component.$.name)">{{ isCollapsed[component.$.name] ? 'Expand' : 'Collapse' }}
		</button>
		<div v-if="component.variable && !isCollapsed[component.$.name]">
			<div v-for="variable in component.variable">
				<Variable 
					:name=variable.$.name 
					:variableMappings="getMappingsForVariable(variable.$.name, component.$.name)"
					@variable-click="handleVariableClick"
				/>
			</div>
		</div>
	</div>
</div>
</template>

<script>
import Variable from './Variable.vue';
export default {
	name: "ComponentView",
	props: {
		propData: Object,        
	},
	data() {
		return {            
			isCollapsed: {},
		} 
	},
	computed: {
		connections: function() {
			return this.propData.model.connection
		},
		components: function() {
			return this.propData.model.component
		}
	},
	watch: {
		components: {
			handler(newVal) {
				this.initializeIsCollapsed();
			},
			immediate: true,
		},
	},
	methods: {
		getMappingsForVariable(variableName, componentName) {
			let mappings = [];
			for (let connection of this.connections) {
				if (connection.map_components[0].$.component_1 == componentName) {
					for (let mapping of connection.map_variables) {
						if (mapping.$.variable_1 == variableName) {
							mappings.push({component: connection.map_components[0].$.component_2,
													variable: mapping.$.variable_2});
						}
					}
				} else if (connection.map_components[0].$.component_2 == componentName) {
					for (let mapping of connection.map_variables) {
						if (mapping.$.variable_2 == variableName) {
							mappings.push({component: connection.map_components[0].$.component_1,
													variable: mapping.$.variable_1});
						}
					}
				}
			}
			return mappings;
		},
		initializeIsCollapsed() {
			this.isCollapsed = {};
			for (const component of this.components) {
				this.isCollapsed[component.$.name] = false;
			}
		},
		toggleCollapse(componentName) {
			this.isCollapsed[componentName] = !this.isCollapsed[componentName];
		},
		handleVariableClick(clickedVariableName, parentComponent, variableMappings) {
			console.log("emitting from ComponentView");
			this.$emit('variable-click', clickedVariableName, parentComponent, variableMappings);
		},
	},
	components: {
		Variable,
	}
}
</script>

<style>
.container {
	display: flex;
	background: antiquewhite; 
	margin: 1em;
	justify-content: center;
	padding: 1em;
	border-radius: 25px;
	gap: 10px 20px;
}

.component {
	position: relative;
	flex: 1;
	flex-wrap: wrap;
	flex-direction: row;
	display: flex;
}

.name {
	padding: 10px 0;	
}

.collapse-button {
	top: 10px;
	right: 10px;
}

.collapsed {
	height: fit-content;
}

</style>
