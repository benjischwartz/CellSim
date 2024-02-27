<template>
    <div class="treecontainer">
        <Treeview 
            :node="propData" 
            :components="components" 
            :connections="connections"
            :highlighted="highlighted"
            :source_coords="source_coords"
            @variable-hover="handleVariableHover"
        />
    </div>
</template>

<script>
import Treeview from './Treeview.vue'
export default {
    name: "TreeContainer",
    components: {
        Treeview
    },
    data() {
        return {
            highlighted: [],
            source_coords: {},
        };
    },
    props: {
        propData: Object,
        components: Object,
        connections: Object,
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
        }
    }
}
</script>

<style>
.treecontainer {
  display: flex;
  flex-grow: 1;
  background: lightcyan;
  padding: 1em;
  border-radius: 25px;
  width: 90%;
  max-width: 2400px;
}
</style>
