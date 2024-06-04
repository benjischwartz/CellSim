<template>
<div class="container">
	<button class="collapse-all-button" @click="toggleContainerCollapse">
      {{ isContainerCollapsed ? 'Expand All' : 'Collapse All' }}
    </button>
	<button class="add-component-button" @click="showComponentPopup = true">
		Add component
    </button>
		<div v-if="showComponentPopup" class="popup"> Add Component
			<input type="text" v-model="componentName" placeholder="Enter component name" @keydown.enter="addComponent(componentName)">
			<button @click="addComponent(componentName)">Done</button>
		</div>
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
					:component=component.$.name
					:variableMappings="getMappingsForVariable(variable.$.name, component.$.name)"
					@variable-click="handleVariableClick"
				/>
			</div>
		</div>
		<!-- Testing menu -->
		<div class="menu-container">
			<button @click="toggleMenu(component.$.name)" class="menu-button">. . .</button>
			<div v-if="showMenu[component.$.name]" class="dropdown-menu">
				<button
					@click="showNestedComponentPopup = true;
					parentComponent=component.$.name
					showMenu[component.$.name]=false">
					Add Component
				</button>
				<button 
					@click="showVariablePopup = true;
					componentName=component.$.name
					showMenu[component.$.name]=false">
					Add Variable
				</button>
				<button @click="action3">Set Name</button>
				<button 
					@click="showEquationPopup = true;
					componentName=component.$.name
					showMenu[component.$.name]=false">
					Set Equation
				</button>
			</div>
		</div>

	</div>
	<div v-if="showVariablePopup" class="popup"> Add Variable
		<input type="text" v-model="variableName" placeholder="Enter variable name" @keydown.enter="addVariable(componentName, variableName)">
		<button @click="addVariable(componentName, variableName)">Done</button>
	</div>
	<div v-if="showNestedComponentPopup" class="popup"> Add Component
		<input type="text" v-model="componentName" placeholder="Enter component name" @keydown.enter="addNestedComponent(componentName, parentComponent)">
		<button @click="addNestedComponent(componentName, parentComponent)">Done</button>
	</div>
	<div v-if="showEquationPopup" class="popup"> Add Equation
		<textarea type="text" v-model="equation" placeholder="Enter equation" @keydown.enter="addEquation(componentName, equation)"></textarea>
		<button @click="addEquation(componentName, equation)">Done</button>
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
			isContainerCollapsed: false,
			showComponentPopup: false,
			showNestedComponentPopup: false,
			showVariablePopup: false,
			showEquationPopup: false,
			componentName: '',
			parentComponent: '',
			variableName: '',
			equation: '',
			showMenu: {},
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
				this.initializeToggleMenu();
			},
			immediate: true,
		},
	},
	methods: {
		initializeToggleMenu() {
			this.showMenu = {};
			for (const component of this.components) {
				this.showMenu[component.$.name] = false;
			}
		},
		toggleMenu(componentName) {
			this.showMenu[componentName] = !this.showMenu[componentName];
		},
		getMappingsForVariable(variableName, componentName) {
			let mappings = [];
			if (this.connections) {
				for (let connection of this.connections) {
					if (connection.$.component_1 == componentName) {
					//if (connection.map_components[0].$.component_1 == componentName) {
						for (let mapping of connection.map_variables) {
							if (mapping.$.variable_1 == variableName) {
								mappings.push({component: connection.$.component_2,
												variable: mapping.$.variable_2});
								//mappings.push({component: connection.map_components[0].$.component_2,
												//variable: mapping.$.variable_2});
							}
						}
					} else if (connection.$.component_2 == componentName) {
					//} else if (connection.map_components[0].$.component_2 == componentName) {
						for (let mapping of connection.map_variables) {
							if (mapping.$.variable_2 == variableName) {
								mappings.push({component: connection.$.component_1,
												variable: mapping.$.variable_1});
								//mappings.push({component: connection.map_components[0].$.component_1,
												//variable: mapping.$.variable_1});
							}
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
		toggleContainerCollapse() {
			// Toggle the overall container collapse state
			this.isContainerCollapsed = !this.isContainerCollapsed;
			
			// Update collapse state for all child components accordingly
			for (const component of this.components) {
				this.isCollapsed[component.$.name] = this.isContainerCollapsed;
			}
		},
		addComponent(componentName) {
			console.log('Component name:', this.componentName);
			this.showComponentPopup = false;
			this.componentName = '';
			this.$emit("add-component", componentName);
		},
		addNestedComponent(componentName, parentComponent) {
			this.showNestedComponentPopup = false;
			this.componentName = '';
			this.parentComponent = '';
			this.$emit("add-nested-component", componentName, parentComponent);
		},
		addVariable(componentName, variableName) {
			this.showVariablePopup = false;
			this.componentName = '';
			this.variableName = '';
			this.$emit("add-variable", componentName, variableName);
		},
		addEquation(componentName, equation) {
			this.showEquationPopup = false;
			this.componentName = '';
			this.equation = '';
			this.$emit("add-equation", componentName, equation);
		}
	},
	components: {
		Variable,
	}
}
</script>

<style>
.container {
	position: relative;
	display: flex;
	background: antiquewhite; 
	margin: 1em;
	justify-content: center;
	padding: 1em;
	border-radius: 25px;
	gap: 10px 20px;
	width: 800px
}

.container > div {
  font-size: 1vw;
  padding: .5em;
  background: silver;
  flex-shrink: 1;
  border: 1px solid black;
  border-radius: 25px;
}

.container > .popup {
  position: absolute;
  top: 10%;
  left: 50%;
  transform: translate(-50%, -50%);
  background: whitesmoke;
  padding: 100px;
  border: 1px solid black;
  border-radius: 50px; 
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
	padding: 5px; 
	font-size: 12px; 
	border-radius: 3px; 
	cursor: pointer;
}

.collapse-all-button {
  position: absolute; 
  top: 5px; 
  right: 5px; 
  padding: 5px; 
  font-size: 12px; 
  border-radius: 3px; 
}

.add-component-button {
  position: absolute; 
  top: 5px; 
  left: 5px; 
  padding: 5px;
  font-size: 12px; 
  border-radius: 3px; 
}

.add-variable-button {
  position: relative; 
  padding: 5px; 
  font-size: 12px; 
  border-radius: 3px; 
}

.add-nested-component-button {
  position: relative; 
  padding: 5px; 
  font-size: 12px; 
  border-radius: 3px; 
}

.collapsed {
	height: fit-content;
}
.menu-container {
  position: relative;
  display: inline-block;
}

.menu-button {
  padding: 10px 20px;
  border: none;
  cursor: pointer;
  top: 5px; 
  right: 5px; 
  padding: 5px; 
  font-size: 12px; 
  border-radius: 3px; 
}

.dropdown-menu {
  display: flex;
  flex-direction: column;
  position: absolute;
  top: 100%;
  left: 0;
  background-color: white;
  border: 1px solid #ccc;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.15);
  z-index: 1000;
}

.dropdown-menu button {
  bottom: 5px; 
  left: 5px; 
  padding: 5px;
  font-size: 12px; 
  border-radius: 3px
}
</style>
