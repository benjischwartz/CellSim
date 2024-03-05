<template>
    <div class="treecontainer">
        <Treeview 
            :node="propData" 
            :components="components" 
            :connections="connections"
            :highlighted="highlighted"
            :source_coords="source_coords"
            :isCollapsed="isCollapsed"
            @variable-hover="handleVariableHover"
        />
        <!-- <div v-for="component in components">
            <div v-if="!isComponentInGroup(component)">
                <div class="component"> {{  component.$.name }}
                    <div v-for="component in components">
                        <div v-if="component.variable">
                            <div v-for="variable in component.variable">
                                <Variable 
                                    :name="variable.$.name"
                                    :component="component.$.name"
                                />
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div> -->
    </div>
</template>

<script>
import Treeview from './Treeview.vue';
import Variable from './Variable.vue';
export default {
    name: "TreeContainer",
    components: {
        Treeview,
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
  background: silver;
  display: flex;
  justify-content: center;
  border-radius: 25px;
  border: 1px solid black;
  padding: 10px;
  margin: 10px;
  width: 90%;
  max-width: 2400px;
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
