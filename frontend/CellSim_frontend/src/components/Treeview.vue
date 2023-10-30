<template>
<!--
    <div>
        <div
            :style="{'margin-left': `${depth*20}px`}"
        >{{node.$.component}}</div>
        <Treeview 
            v-for="child in node.component_ref" 
            :key="child.$.component" 
            :node="child" 
            :depth="depth+1"
        />
    </div>
-->
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
.container {
  display: flex;
  background: antiquewhite;
  flex-wrap: wrap;
  margin: 10em;
  border: 1px solid black;
  border-radius: 25px;
}

.component {
  display: flex;
  flex: 1;  
  flex-wrap: wrap;
  width: 250px;
  border: 1px solid black;
  border-radius: 25px;
  margin: 1em;
}

.variable {
  background: lightblue;
  margin: .5em;
  border: 1px solid black;
  border-radius: 25px;
}

.container > div {
  font-size: 1vw;
  padding: .5em;
  background: silver;
  border: 1px solid black;
  border-radius: 25px;
  flex-wrap: wrap;
}

</style>
