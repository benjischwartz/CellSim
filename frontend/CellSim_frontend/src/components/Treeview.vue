<template>
    <div class="component" > {{ node.$.component }} 
      <div v-for="component in components">
        <div v-if="component.$.name === node.$.component">
          <div v-if="component.variable">
            <div class="variable" v-for="variable in component.variable">
               {{ variable.$.name }}
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
    />
    </div>

</template>

<script>
export default {
    name: "Treeview",
    props: {
        node: Object,
        depth: {
            type: Number,
            default: 0
        },
        components: Object,
    }
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
  max-width: 1200px;
}

.variable {
  background: lightblue;
  margin: 5px;
  border: 1px solid black;
  border-radius: 25px;
}

</style>
