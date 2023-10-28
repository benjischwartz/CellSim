<template>
    <div>
        <h3>Tree view</h3>
        <Treeview :node="propData" />
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
        propData: Object
    },
    data() {
        return {
            treeData : {
                name: "Root",
                children: [
                    { name: "Child 1" },
                    { name: "Child 2" },
                    {
                        name: "Child 3",
                        children: [
                        {
                            name: "Sub Child 1",
                            children: [{ name: "Inner Sub Child 1" }, { name: "Inner Sub Child 2" }]
                        },
                        { name: "Sub Child 2" },
                        { name: "Sub Child 3" },
                        {
                            name: "Sub Child 4",
                            children: [{ name: "Inner Sub Child 3" }, { name: "Inner Sub Child 4" }]
                        }
                        ]
                    }
                ]
            }
        }
    }
}
</script>
