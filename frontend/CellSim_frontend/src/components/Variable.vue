<template>
<div class=variable @mouseenter="showMappings" @mouseleave="hideMappings">
    {{ name }}
    <div v-if="isHovering">
        <div v-for="mapping in variableMappings">
            <VariableMapping :mapping="mapping"/>
        </div>
    </div>
</div>
</template>

<script>
import VariableMapping from './VariableMapping.vue';

export default {
    name: 'Variable',
    props: {
        name: {
            type: String,
            required: true
        },
        variableMappings: {
            type: Array
        }
    },
    components: {
        VariableMapping
    },
    data() {
        return {
            isHovering: false
        };
    },
    methods: {
        showMappings() {
            this.isHovering = true;
        },
        hideMappings() {
            this.isHovering = false;
        }
    }
}
</script>

<style>
.variable {
  background: lightblue;
  margin: 5px;
  border: 1px solid black;
  border-radius: 25px;
  padding: 0.5em;
}

.variable:hover {
  background: greenyellow;
}
</style>
