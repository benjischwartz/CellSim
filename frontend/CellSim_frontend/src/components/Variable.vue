<template>
<div class=variable 
	:class="{ 'highlighted': isHighlighted }"
	@mouseenter="showMappings" 
	@mouseleave="hideMappings"
	@click="handleClick">
	{{ name }}
	<div v-if="this.inComponentView" class="menu-container">
		<button @click="toggleMenu" class="menu-button">...</button>
		<div v-if="showMenu" class="dropdown-menu">
			<button @click="toggleUnitsMenu">Set Units</button>
			<div v-if="showUnitsMenu" class ="units-dropdown">
				<ul>
					<button v-for="unit in allUnits" @click="handleSetUnitsClick(unit.$.name)"> {{ unit.$.name }}</button>
				</ul>
			</div>
			<button @click="toggleInitialValueMenu">Set Initial Value</button>
			<div v-if="showInitialValueMenu">
				<input type="text" name="initial_value" v-model="initial_value" placeholder="Initial Value" />
				<button @click="handleSetInitialValueClick(initial_value)">Set</button>
			</div>
			<button @click="toggleEquivComponentsMenu">Add Equivalence</button>
			<div v-if="showEquivComponentsMenu" class="units-dropdown">
				<ul>
					<button v-for="c in this.propData.model.component" @click="toggleEquivVariablesMenu(c.$.name)"> 
						{{ c.$.name }}
						<div v-if="showEquivVariablesMenu[c.$.name]" class="units-dropdown">
							<ul>
								<button v-for="v in c.variable" @click="handleSetEquivalenceClick(c.$.name, v.$.name)"> 
									{{ v.$.name }}
								</button>
							</ul>
						</div>
					</button>
				</ul>
			</div>
			<button @click="toggleInterfaceTypeMenu">Set Interface Type
				<div v-if="showInterfaceTypeMenu" class="units-dropdown">
					<button @click="handleSetInterfaceTypeClick('public')">Public</button>
					<button @click="handleSetInterfaceTypeClick('private')">Private</button>
					<button @click="handleSetInterfaceTypeClick('public_and_private')">Public and Private</button>
				</div>
			</button>
		</div>
	</div>
</div>
</template>

<script>
import VariableMapping from './VariableMapping.vue';

export default {
	name: 'Variable',
	props: {
		name: String,
		component: String,
		units: Array,
		variableMappings: Array,
		highlighted: Array,
		propData: Object,
		inComponentView: Boolean,
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
		allUnits() {
			let myUnits = [];
			if (this.units) {
				myUnits = this.units;
			}
			myUnits.push({
				"$": {
				"name": "dimensionless"
				},
			});
			return myUnits;
		},
	},
	watch: {
		propData: {
			handler() {
				this.updateShowEquivVariablesMenu();
			},
			immediate: true,
		},
	},
	components: {
		VariableMapping
	},
	data() {
		return {
			isHovering: false,
			showMenu: false,
			showUnitsMenu: false,
			showInitialValueMenu: false,
			showEquivComponentsMenu: false,
			showEquivVariablesMenu: {},
			showInterfaceTypeMenu: false,
		};
	},
	created() {
		this.updateShowEquivVariablesMenu();
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
		updateShowEquivVariablesMenu() {
			if (this.propData) {
				this.propData.model.component.forEach(component => {
					this.showEquivVariablesMenu[component.$.name] =false;
				});
			}
		},
		handleClick() {
			this.$emit('variable-click', this.name, this.component, this.variableMappings, this.units);
		},
		handleSetUnitsClick(unitsName) {
			this.showMenu = false;
			this.showUnitsMenu = false;
			this.$emit('set-units-click', this.component, this.name, unitsName);
		},
		handleSetInitialValueClick(initialValue) {
			this.showMenu = false;
			this.showUnitsMenu = false;
			this.showInitialValueMenu = false;
			this.$emit('set-initial-value-click', this.component, this.name, initialValue);
		},
		handleSetEquivalenceClick(target_component, target_variable) {
			this.showEquivComponentsMenu = false;
			this.showMenu = false;
			this.updateShowEquivVariablesMenu();
			this.$emit('set-equivalence-click', this.component, target_component, this.name, target_variable);
		},
		handleSetInterfaceTypeClick(interface_type) {
			this.showInterfaceTypeMenu = false;
			this.showMenu = false;
			this.$emit('set-interface-type-click', this.component, this.name, interface_type);
		},
		toggleMenu() {
			console.log(this.units)
			this.showMenu = !this.showMenu;
		},
		toggleUnitsMenu() {
			this.showUnitsMenu = !this.showUnitsMenu;
		},
		toggleInitialValueMenu() {
			this.showInitialValueMenu = !this.showInitialValueMenu;			
		},
		toggleEquivComponentsMenu() {
			this.showEquivComponentsMenu = !this.showEquivComponentsMenu;			
		},
		toggleEquivVariablesMenu(componentName) {
			console.log("toggling equiv")
			this.showEquivVariablesMenu[componentName] = !this.showEquivVariablesMenu[componentName];
		},
		toggleInterfaceTypeMenu() {
			this.showInterfaceTypeMenu = !this.showInterfaceTypeMenu;			
		},
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

.units-dropdown {
  position: absolute;
  top: 0;
  left: 100%;
  /* Add your styling here */
}

ul {
  list-style-type: none;
  padding: 0;
  margin: 0;
}

li {
  padding: 8px 12px;
  cursor: pointer;
  /* Add your styling here */
}

li:hover {
  background-color: #f0f0f0;
}
	
</style>
