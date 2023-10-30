<template>
    <div>
        <Treeview :node="propData" :components="components"/>
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
    },
}
</script>
