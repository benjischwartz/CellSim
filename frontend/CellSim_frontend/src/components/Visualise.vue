<template>
<div>
		<h1>Visualize CellML File</h1>
		<input type="file" @change="handleFileUpload" accept=".xml" />
		<br /><br />
		<textarea name="file" v-model="file" placeholder="Or paste File Data Here"></textarea> 
		<br /><br />
		<button @click="sendData()">Visualize</button> <br /><br />
</div>
<br /><br />

<div v-if="clickedVariableInfo">
	<VariableInfo 
		:propVariableName=clickedVariableInfo.name
		:propComponentName=clickedVariableInfo.component
		:propVariableMappings=clickedVariableInfo.mappings
		:propVariableUnits=clickedVariableInfo.units
		:propEquation=clickedVariableInfo.equation
	/>
</div>
<br /><br />

<div v-if="jsData">
	<h2>Component View</h2>
	<ComponentView 
		:propData=jsData 
		@variable-click="showVariableInfo"
	/>
</div>

<div v-if="jsData">
	<h2> Tree View </h2>
	<div class= "group-wrapper">
		<div v-for="group in groups">
			<!-- get containment relationships -->
			<h3> {{ group.relationship_ref[0].$.relationship}} {{ "relationship" }} </h3>
			<div>
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
</div>

</template>

<script>
import TreeContainer from './TreeContainer.vue';
import ComponentView from './ComponentView.vue';
import VariableInfo from './VariableInfo.vue';
import Variable from './Variable.vue';
import axios from 'axios';
import xml2js from 'xml2js';

export default {
	name: 'visualise',
	data() {
		return {
			file: '',
			resultData: null,
			jsData: '',
			isCollapsed: {},
			clickedVariableInfo: {},
		};
	},
	computed: {
		components: function() {
			return this.jsData.model.component;
		},
		connections: function() {
			return this.jsData.model.connection;
		},
		groups: function() {
			return this.jsData.model.group;
		},
	},
	methods: {
		handleFileUpload(event) {
			const selectedFile = event.target.files[0];
			if (selectedFile) {
				const reader = new FileReader();
				reader.onload = (e) => {
					this.file = e.target.result;
				};
				reader.readAsText(selectedFile);
			}
		},
		async sendData() {
			const result = await axios.post('http://localhost:8000/api/visualise', {
				file: this.file,
			});
			this.resultData = result;
			console.warn('function called', this.file);
			xml2js.parseString(this.file, (err, result) => {
				if (err) {
					throw err;
				}
				this.jsData = result;
				const str = JSON.stringify(result, null, 2);
				console.log('js -> %s', str);
			});
		},
		async getEquation(componentName) 
		{
			const result = await axios.post('http://localhost:8000/api/getEquation', {
				component: componentName,
			});
			console.log("get equation result: ", result);
			return result;
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
		showVariableInfo(clickedVariableName, parentComponent, variableMappings, variableUnits, variableEquation) {
			this.clickedVariableInfo['name'] = clickedVariableName;
			this.clickedVariableInfo['component'] = parentComponent;
			this.clickedVariableInfo['mappings'] = variableMappings;
			this.clickedVariableInfo['units'] = variableUnits;
			this.clickedVariableInfo['equation'] = this.getEquation(parentComponent);
			//this.clickedVariableInfo['equation'] = '';
		},
	},
	components: {
		TreeContainer,
		Variable,
		ComponentView,
		VariableInfo,
	},
};

</script>

<style>

.group-wrapper {
	display: flex;
	border-radius: 25px;
	flex-direction: column;
	gap: 10px 20px;
}

</style>
