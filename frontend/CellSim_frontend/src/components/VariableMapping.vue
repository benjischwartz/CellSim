<template>
    <div class='variable-mapping'>
        Mapped Component: {{mapping.component}}
        Mapped Variable: {{mapping.variable}}
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
