<template>
    <div class="treecontainer">
        <Treeview :node="propData" :components="components" :connections="connections"/>
    </div>
</template>

<script>
import Treeview from './Treeview.vue'
export default {
    name: "TreeContainer",
    components: {
        Treeview
    },
    props: {
        propData: Object,
        components: Object,
        connections: Object,
    },
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
