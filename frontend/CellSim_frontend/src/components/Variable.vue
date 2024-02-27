<template>
<div class=variable 
    :class="{ 'highlighted': isHighlighted }"
    @mouseenter="showMappings" 
    @mouseleave="hideMappings">
    {{ name }}
    <!-- Don't need to print mappings anymore
    <div v-if="isHovering">
        <div v-for="mapping in variableMappings">
            <VariableMapping :mapping="mapping"/>
        </div>
    </div>
    -->
    <!-- First attempt at arrow printing
    <svg v-if="isHighlighted" class="arrows">
        <line 
        :x1="source_coords.x" 
        :y1="source_coords.y" 
        :x2="coords.x" 
        :y2="coords.y"
        stroke="black" 
        stroke-width="2"
        />
    </svg>
    -->
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
        component: {
            type: String,
            required: true
        },
        variableMappings: {
            type: Array
        },
        highlighted: {
            type: Array
        },
        source_coords: {
            type: Object,
            required: false
        },
    },
    computed: {
        isHighlighted() {
            if (!this.highlighted) return false;
            console.log("This variable is highlighted");
            console.log("Source coords are", this.source_coords);
            return this.highlighted.some(mapping=> {
                return (mapping.component === this.component && mapping.variable === this.name);
            });
        },
        coords() {
            return this.getPosition();
        },
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
        getPosition() {
            const rect = this.$el.getBoundingClientRect();
            console.log(rect);
            return {
                x: rect.left,
                y: rect.top,
            };
        },
        showMappings() {
            console.log('Mouse entered');
            this.isHovering = true;
            let coords = this.getPosition();
            this.$emit('variable-hover', this.name, coords.x, coords.y, this.variableMappings);
        },
        hideMappings() {
            console.log('Mouse left');
            this.isHovering = false;
            this.$emit('variable-hover', null, null, null, null);
        },
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

.variable.highlighted {
    background: greenyellow;
}

.variable:hover {
  background: greenyellow;
}

.arrows line {
  fill: none;
}
    
</style>
