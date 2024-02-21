<template>
    <div class='variable-mapping'>
        Connection: {{mapping.component_1}} = {{mapping.component_2}}
        Variables: {{mapping.variable_1}} = {{mapping.variable_2}}
        <br><br/>
    </div>
</template>

<script>
export default {
    name: 'VariableMapping',
    props: {
        mapping: {
            type: Object,
            required: true
        }
    }
};
</script>
