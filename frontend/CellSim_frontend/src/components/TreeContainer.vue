<template>
    <div class="treecontainer">
        <TreeView 
            :node="propData" 
            :components="components" 
            :connections="connections"
            :highlighted="highlighted"
            :source_coords="source_coords"
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
            source_coords: {},
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
        handleVariableHover(hoveredVariableName, variable_x, variable_y, hoveredVariableMappings) {
            console.log('In TreeContainer.vue:');
            console.log('Hovered Variable Name:', hoveredVariableName);
            console.log('Hovered Variable x coord:', variable_x);
            console.log('Hovered Variable y coord:', variable_y);
            console.log('Hovered Variable Mappings:', hoveredVariableMappings);
            this.highlighted = [];
            if (hoveredVariableMappings) {
                for (const mapping of hoveredVariableMappings) {
                    this.highlighted.push(mapping);
                }
            }
            this.source_coords.x = variable_x;
            this.source_coords.y = variable_y;
            console.log('highlighted is now: ', this.highlighted);
        },
        isComponentInGroup(my_component) {
            for (const component of this.componentsInGroup) {
                if (my_component.$.name == component.$.component) {
                    console.log("In group: ", my_component);
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
