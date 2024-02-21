<template>
    <div class="component" > {{ node.$.component }} 
      <div v-for="component in components">
        <div v-if="component.$.name === node.$.component">
          <div v-if="component.variable">
            <div class="box">
            <div v-for="variable in component.variable">
                <Variable :name=variable.$.name :variableMappings="getMappingsForVariable(variable.$.name, component.$.name)"/>
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
    />
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
    },
    methods: {
    getMappingsForVariable(variableName, componentName) {
        let mappings = [];
        for (let connection of this.connections) {
            if (connection.map_components[0].$.component_2 == componentName ||
                connection.map_components[0].$.component_1 == componentName) {
                for (let mapping of connection.map_variables) {
                    if (mapping.$.variable_1 == variableName || 
                        mapping.$.variable_2 == variableName) {
                        mappings.push({component_1: connection.map_components[0].$.component_1,
                                        component_2: connection.map_components[0].$.component_2,
                                        variable_1: mapping.$.variable_1,
                                        variable_2: mapping.$.variable_2});
                    }
                }
            }
        }
        return mappings;
    }},
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

</style>
