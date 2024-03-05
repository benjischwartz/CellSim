<template>
    <div class="component" > {{ node.$.component }} 
        <button class="collapse-button2" @click="toggleCollapse(node.$.component)">
            {{ isCollapsed[node.$.component] ? 'Expand' : 'Collapse' }}
        </button>

    <div v-if="!isCollapsed[node.$.component]"> 
      <div v-for="component in components">
        <div v-if="component.$.name === node.$.component">
          <div v-if="component.variable">
            <div class="box">
            <div v-for="variable in component.variable">
                <Variable 
                    :name=variable.$.name
                    :variableMappings="getMappingsForVariable(variable.$.name, component.$.name)"
                    :highlighted="highlighted"
                    :component="component.$.name"
                    @variable-hover="handleVariableHover"
                />
            </div>
            </div>
         </div>
        </div>
      </div>

        <Treeview 
            v-for="child in node.component_ref" 
            :key="child.$.component" 
            :node="child" 
            :depth="depth+1"
            :components="components"
            :connections="connections"
            :highlighted="highlighted"
            :isCollapsed="isCollapsed"
            @variable-hover="handleVariableHover"
        />
    </div>

    </div>
</template>

<script>
import Variable from './Variable.vue';
import VariableMapping from './VariableMapping.vue';
export default {
    name: "Treeview",
    props: {
        node: Object,
        depth: {
            type: Number,
            default: 0
        },
        components: Object,
        connections: Object,
        highlighted: Object,
        isCollapsed: Object,
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
    handleVariableHover(hoveredVariableName, variable_x, variable_y, hoveredVariableMappings) {
        console.log('Hovered Variable Name:', hoveredVariableName);
        console.log('Hovered Variable Mappings:', hoveredVariableMappings);
        console.log('highlighted:', this.highlighted);
        this.$emit('variable-hover', hoveredVariableName, variable_x, variable_y, hoveredVariableMappings);
    },
    toggleCollapse(componentName) {
        this.isCollapsed[componentName] = !this.isCollapsed[componentName];
        // const childComponents = this.node.component_ref;
        // if (childComponents) {
        //     childComponents.forEach(child => {
        //         this.isCollapsed[child.$.component] = !this.isCollapsed[child.$.component];
        //     });
        // }
        // Recursively toggle the collapse state of all child components
        const toggleChildCollapse = (node) => {
            if (node.component_ref) {
                for (const child of node.component_ref) {
                    this.isCollapsed[child.$.component] = this.isCollapsed[componentName];
                    toggleChildCollapse(child);
                }
            }
        };

        toggleChildCollapse(this.node);
    },
    },
    components: {
        Variable,
        VariableMapping,
    },
}
</script>

<style>

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
