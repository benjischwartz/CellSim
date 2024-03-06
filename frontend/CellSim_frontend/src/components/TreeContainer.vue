<template>
<div class="treecontainer">
	<TreeView 
		:node="propData" 
		:components="components" 
		:connections="connections"
		:highlighted="highlighted"
		:isCollapsed="isCollapsed"
		@variable-hover="handleVariableHover"
	/>
</div>
</template>

<script>
import TreeView from './TreeView.vue'; 
import Variable from './Variable.vue';
export default {
	name: "TreeContainer",
	components: {
		TreeView,
		Variable,
	},
	data() {
		return {
			highlighted: [],
			isCollapsed: {}
		};
	},
	props: {
		propData: Object,
		components: Object,
		connections: Object,
		componentsInGroup: Array,
	},
	methods: {
		handleVariableHover(hoveredVariableName, hoveredVariableMappings) {
			this.highlighted = [];
			if (hoveredVariableMappings) {
				for (const mapping of hoveredVariableMappings) {
					this.highlighted.push(mapping);
				}
			}
		},
		isComponentInGroup(my_component) {
			for (const component of this.componentsInGroup) {
				if (my_component.$.name == component.$.component) {
					return true;
				}
			}
			return false;
		},
	},
}
</script>

<style>
.treecontainer {
  display: flex;
  flex-grow: 20;
  background: lightcyan;
  padding: 1em;
  border-radius: 25px;
  width: 90%;
  max-width: 2400px;
}

.component {
  position: relative;
  flex: 1;
  flex-wrap: wrap;
  flex-direction: row;
  display: flex;
}

.component > div {
  padding: 1em;
  margin: 10px;
}

.box {
  display: flex;
  padding: 1em;
}

.collapse-button2 {
	position: absolute;
	top: 10px;
	right: 10px;
}

.collapsed {
	height: fit-content;
}

</style>
