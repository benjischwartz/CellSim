<template>
    <div>
        <div
            :style="{'margin-left': `${depth*20}px`}"
        >{{node.name}}</div>
        <Treeview 
            v-for="child in node.children" 
            :key="child.name" 
            :node="child" 
            :depth="depth+1"
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
        }
    }
}
</script>
