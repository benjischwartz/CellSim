<template> 
  <div>
    <h1>Create CellML File</h1>
    <!-- Add text boxes for model name and model id -->
    <input type="text" name="model_name" v-model="model_name" placeholder="Model Name" />
    <input type="text" name="model_id" v-model="model_id" placeholder="Model ID" @keydown.enter="sendModel()">
    <button @click="sendModel()">Create</button> <br /><br />
  </div> <br /><br />

  <!-- Display the result data in a panel -->
  <div v-if="xmlData">
    <h2> {{ model_name }}</h2>
    <pre>{{ formattedData }}</pre>

    <div v-if="jsData.model.component">
    <h2>Component View</h2>
    <ComponentView 
      :propData=jsData
      @add-component="addComponent"
      @add-nested-component="addNestedComponent"
      @add-variable="addVariable"
      @add-equation="addEquation"
      @set-units-click="setUnits"
    /> </div>

    <div v-if="this.jsData && encapsulation">
      <h2> Encapuslation </h2>
      <div class= "group-wrapper">
        <div v-for="group in encapsulation">
          <div v-for="component_ref in group.component_ref">
            <TreeContainer 
              :propData=component_ref 
              :components=components 
              :connections=connections
              :componentsInGroup=getComponentsInGroup(group)
              @variable-click="showVariableInfo" 
            />
          </div>
        </div>
      </div>
    </div>

    <div v-if="this.jsData && containment">
      <h2> Containment </h2>
      <div class= "group-wrapper">
        <div v-for="group in containment">
          <div v-for="component_ref in group.component_ref">
            <TreeContainer 
              :propData=component_ref 
              :components=components 
              :connections=connections
              :componentsInGroup=getComponentsInGroup(group)
              @variable-click="showVariableInfo" 
            />
          </div>
        </div>
      </div>
    </div>

    <button>Add Metadata</button> <br />
    <button @click="showUnitsInput=true">Add Units</button> <br />
    <div v-if="showUnitsInput">
      <input type="text" name="units_name" v-model="units_name" placeholder="Units Name" /> <br />
      <select v-model="prefix_enum">
        <option disabled value="">Select a prefix</option>
        <option v-for="(item, index) in prefix_enums" :key="index" :value="index">
          {{ item }}
        </option>
      </select>
      <br />
      <select v-model="si_enum">
        <option disabled value="">Select a unit</option>
        <option v-for="(item, index) in si_enums" :key="index" :value="index">
          {{ item }}
        </option>
      </select>
      <br />
      <input type="text" name="exponent" v-model="exponent" placeholder="Exponent" /> <br /> 
      <input type="text" name="multiplier" v-model="multiplier" placeholder="Multiplier" /> <br /> 
      <button @click="addUnits(units_name)">Add</button><br /><br />
    </div>
    <button @click="showComponentInput=true">Add Component</button> <br />
    <div v-if="showComponentInput">
      <input type="text" name="component_name" v-model="component_name" placeholder="Component Name" />
      <button @click="addComponent(component_name)">Add</button><br /><br />
    </div>
    <button>Set Model Name </button> <br />
    <button @click="exportXML()">Export </button> <br />
  </div>
  <br /><br />
  <div v-if="issueList"> <h2> Issue List: </h2> <br /> {{ issueList }}</div>


</template>

<script>
import ComponentView from './ComponentView.vue';
import TreeContainer from './TreeContainer.vue';
import axios from 'axios';
import xml2js from 'xml2js';

let separator = "*separator*";
export default {
  name: "Create",
  data()
  {
    return {
      file: "",
      model_name: "",
      model_id: "",
      units_name: "",
      prefix_enum: "",
      si_enum: "",
      component_name: "",
      edit_type: "",
      visualise: false,
      formattedData: "", // data property to store the formatted result
      xmlData: "", // data property to store xml result
      issueList: "", // data property to store validator result 
      jsData: "",
      components: [],
      items: [],
      groups: [],
      showComponentInput: false,
      showUnitsInput: false,
      prefix_enums: [
        'YOTTA',
        'ZETTA',
        'EXA',
        'PETA',
        'TERA',
        'GIGA',
        'MEGA',
        'KILO',
        'HECTO',
        'DECA',
        'DECI',
        'CENTI',
        'MILLI',
        'MICRO',
        'NANO',
        'PICO',
        'FEMTO',
        'ATTO',
        'ZEPTO',
        'YOCTO'
      ],
      si_enums: [
        'AMPERE',
        'BECQUEREL',
        'CANDELA',
        'COULOMB',
        'DIMENSIONLESS',
        'FARAD',
        'GRAM',
        'GRAY',
        'HENRY',
        'HERTZ',
        'JOULE',
        'KATAL',
        'KELVIN',
        'KILOGRAM',
        'LITRE',
        'LUMEN',
        'LUX',
        'METRE',
        'MOLE',
        'NEWTON',
        'OHM',
        'PASCAL',
        'RADIAN',
        'SECOND',
        'SIEMENS',
        'SIEVERT',
        'STERADIAN',
        'TESLA',
        'VOLT',
        'WATT',
        'WEBER'
      ],
      exponent: '',
      multiplier: '',
    };
  },
  computed: {
    components: function() {
      return this.jsData.model.component;
    },
    connections: function() {
      // TODO: Update this with connections
      //return this.jsData.model.connection;
      return []
    },
    units: function() {
      // TODO: Update this with connections
      //return this.jsData.model.connection;
      return this.jsData.model.units;
    },
    encapsulation: function() {
      return this.jsData.model.encapsulation;
    },
    containment: function() {
      return this.jsData.model.containment;
    },
  },
  methods:{
    async sendModel()
    {
      let result = await axios.post("http://localhost:8000/api/create", {
        model_name: this.model_name,
        model_id: this.model_id,
      })
      console.log("sending model");

      // Separate the response data into XML, formatted data, and issue list
      const separator = '*separator*';
      const responseText = result.data;
      const parts = responseText.split(separator);
      if (parts.length === 3) {
        this.xmlData = parts[0];
        this.formattedData = parts[1];
        this.issueList = parts[2];
      } else {
        console.error('Unexpected response format');
      }
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
    },
    async addUnits(units_name)
    {
      console.log('Units Name:', this.units_name);
      console.log('Selected Prefix:', this.prefix_enum);
      console.log('Selected Unit:', this.si_enum);
      let exp = parseFloat(this.exponent);
      if (isNaN(exp)) {
        exp = 1.0
      }
      let mul = parseFloat(this.multiplier);
      if (isNaN(mul)) {
        mul = 1.0
      }
      console.log('Exponent: ', exp);
      console.log('Multiplier:', mul);
      // Convert selected enum values to strings
      const selectedPrefixString = String(this.prefix_enum);
      const selectedUnitString = String(this.si_enum);

      let result = await axios.post("http://localhost:8000/api/edit", {
        edit_type: "add_units",
        units_name: units_name,
        prefix_enum: selectedPrefixString,
        si_enum: selectedUnitString,
        file: this.xmlData,
        model_name: this.model_name,
        model_id: this.model_id,
        exponent: exp,
        multiplier: mul,
      })
      // Separate the response data into XML, formatted data, and issue list
      const responseText = result.data;
      const parts = responseText.split(separator);
      if (parts.length === 3) {
        this.xmlData = parts[0];
        this.formattedData = parts[1];
        this.issueList = parts[2];
      } else {
        console.error('Unexpected response format');
      }
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
      this.showUnitsInput = false;
    },
    async setUnits(componentName, variableName, unitsName)
    {
      let result = await axios.post("http://localhost:8000/api/edit", {
        edit_type: "set_units",
        units_name: unitsName,
        component_name : componentName,
        variable_name: variableName,
        file: this.xmlData,
        model_name: this.model_name,
        model_id: this.model_id,
      })
      // Separate the response data into XML, formatted data, and issue list
      const responseText = result.data;
      const parts = responseText.split(separator);
      if (parts.length === 3) {
        this.xmlData = parts[0];
        this.formattedData = parts[1];
        this.issueList = parts[2];
      } else {
        console.error('Unexpected response format');
      }
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
      this.showUnitsInput = false;
    },
    async addComponent(componentName)
    {
      console.log("Adding component: ", componentName);
      let result = await axios.post("http://localhost:8000/api/edit", {
        edit_type: "add_component",
        component_name: componentName,
        file: this.xmlData,
        model_name: this.model_name,
        model_id: this.model_id,
      })
        
      // Separate the response data into XML, formatted data, and issue list
      const separator = '*separator*';
      const responseText = result.data;
      const parts = responseText.split(separator);
      if (parts.length === 3) {
        this.xmlData = parts[0];
        this.formattedData = parts[1];
        this.issueList = parts[2];
      } else {
        console.error('Unexpected response format');
      }
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
      this.showComponentInput = false;
    },
    async addNestedComponent(componentName, parentComponent)
    {
      let result = await axios.post('http://localhost:8000/api/edit', {
        edit_type: "add_child_component",
        parent_component_name: parentComponent,
        child_component_name: componentName,
        file: this.xmlData,
      });
      
      // Separate the response data into XML, formatted data, and issue list
      const responseText = result.data;
      const parts = responseText.split(separator);
      if (parts.length === 3) {
        this.xmlData = parts[0];
        this.formattedData = parts[1];
        this.issueList = parts[2];
      }
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
      this.showComponentInput = false;
    },
    async addVariable(componentName, variableName)
    {
      let result = await axios.post('http://localhost:8000/api/edit', {
        edit_type: "add_variable",
        component_name: componentName,
        variable_name: variableName,
        file: this.xmlData,
      })
      
      // Separate the response data into XML, formatted data, and issue list
      const responseText = result.data;
      const parts = responseText.split(separator);
      if (parts.length === 3) {
        this.xmlData = parts[0];
        this.formattedData = parts[1];
        this.issueList = parts[2];
      }
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
      console.log('')
    },
    async addEquation(componentName, equation)
    {
      let result = await axios.post('http://localhost:8000/api/edit', {
        edit_type: "add_equation",
        component_name: componentName,
        equation: equation,
        file: this.xmlData,
      })
      
      // Separate the response data into XML, formatted data, and issue list
      const responseText = result.data;
      const parts = responseText.split(separator);
      if (parts.length === 3) {
        this.xmlData = parts[0];
        this.formattedData = parts[1];
        this.issueList = parts[2];
      }
      xml2js.parseString(this.xmlData, (err, result) => {
        if (err) {
          throw err;
        }
        this.jsData = result;
        const str = JSON.stringify(result, null, 2);
        console.log('js -> %s', str);
      });
    },
    getComponentsInGroup(group) {
      let components = [];
      const getComponentsRecursive = (node) => {
        if (node.component_ref) {
          for (const child of node.component_ref) {
            components.push(child);
            getComponentsRecursive(child);
          }
        }
      };
      getComponentsRecursive(group);
      return components;
    },
    exportXML() {
      const blob = new Blob([this.xmlData], { type: 'text/xml'});
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = `${this.model_name}.cellml`;
      a.click();
      URL.revokeObjectURL(url);
    },
  },

  components: {
    ComponentView,
    TreeContainer,
  },
};
</script>