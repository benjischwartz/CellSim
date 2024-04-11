<template>
<div>
	<h1>Visualize CellML File</h1>
	<input type="file" @change="handleFileUpload" accept=".xml" />
	<br /><br />
	<textarea name="file" v-model="file" placeholder="Paste File Data Here" class="upload-section"></textarea> 
	<br /><br />
	<button @click="sendData()">Visualize</button> <br /><br />
</div>
<br /><br />

<div>
	<div class = "dropzone"
		@dragover.prevent="dragOver"
		@drop.prevent="dropFiles"
		@click="selectFiles">
		<p v-if="files.length === 0">Upload dependencies: drag files here or click to upload</p>
		<ul v-else>
			<li v-for="(file, index) in files" :key="index">
				{{ file.name }}
				<button @click="removeFile(index)">Remove</button>
			</li>
		</ul>
	</div>
	<input
		type="file"
		ref="fileInput"
		multiple
		style="display: none"
		@change="handleFileInputChange" />
	<button @click="submitFiles">Submit</button>
</div>
<br /><br />
<div v-if="this.filesUploaded.length != 0">
	<h2> Files uploaded: </h2>
	<div v-for="file in filesUploaded">
	{{ file.name }}
	</div>	
</div>
<br /><br />
<br /><br />

<div v-if="this.jsData">
	<VariableInfo 
		:propVariableName=clickedVariableInfo.name
		:propComponentName=clickedVariableInfo.component
		:propVariableMappings=clickedVariableInfo.mappings
		:propVariableUnits=clickedVariableInfo.units
		:propEquation=clickedVariableInfo.equation
	/>
</div>
<br /><br />

<div v-if=errorData>
	<h2> Error! </h2>{{ errorData }}
</div>

<div v-if="jsData">
	<h2>Component View</h2>
	<ComponentView 
		:propData=jsData 
		@variable-click="showVariableInfo"
		@add-component="addComponent"
		@add-nested-component="addNestedComponent"
		@add-variable="addVariable"
	/>
</div>
<br /><br />

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
			files: [],
			filesUploaded: [],
			resultData: null,
			errorData: null,
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
		encapsulation: function() {
			return this.jsData.model.encapsulation;
		},
		containment: function() {
			return this.jsData.model.containment;
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
			try {
				const result = await axios.post('http://localhost:8000/api/visualise', {
					file: this.file,
				});
				console.log(result)
				console.log('flattened model %s', result.data);
				this.resultData = result.data;
				this.file = result.data;
				xml2js.parseString(result.data, (err, result) => {
					if (err) {
						throw err;
					}
					this.jsData = result;
					const str = JSON.stringify(result, null, 2);
					console.log('js -> %s', str);
				});
			} catch (error) {
				console.error('An error occurred:', error);
				this.errorData = error.response.data
				this.jsData = null
			}
		},
		async getEquation(componentName) 
		{
			console.log("component name %s", componentName);
			const result = await axios.post('http://localhost:8000/api/getEquation', {
				component: componentName,
			});
			console.log("get equation result: ", result.data);
			return result.data;
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
		async showVariableInfo(clickedVariableName, parentComponent, variableMappings, variableUnits, variableEquation) {
			this.clickedVariableInfo['name'] = clickedVariableName;
			this.clickedVariableInfo['component'] = parentComponent;
			this.clickedVariableInfo['mappings'] = variableMappings;
			this.clickedVariableInfo['units'] = variableUnits;
			this.clickedVariableInfo['equation'] = await this.getEquation(parentComponent);
		},
		dragOver(event) {
			event.preventDefault();
		},
		dropFiles(event) {
			event.preventDefault();
			this.addFiles(event.dataTransfer.files);
		},
		selectFiles() {
			this.$refs.fileInput.click();
		},
		handleFileInputChange(event) {
			this.addFiles(event.target.files);
		},
		addFiles(newFiles) {
			for (let i = 0; i < newFiles.length; i++) {
				this.files.push(newFiles[i]);
			}
		},
		removeFile(index) {
			this.files.splice(index, 1);
			this.filesUploaded.splice(index, 1);
		},
		async submitFiles() {
			const formData = new FormData();
			this.errorData = null;
			for (let i = 0; i < this.files.length; i++) {
				formData.append('files[]', this.files[i]);
				this.filesUploaded.push(this.files[i]);
			}
			try {
				const result = await axios.post('http://localhost:8000/api/uploadDependencies', formData, {
					headers: {
						'Content-Type': 'multipart/form-data'
					}
				});
				console.log('Upload successful:', result);
				this.files = []
			} catch (error) {
				console.error('Error uploading files: ', error);
			}
			
		},
		async addComponent(componentName)
		{
			console.log("Adding component!!!")
			try {
				const result = await axios.post('http://localhost:8000/api/edit', {
					edit_type: "add_component",
					component_name: componentName,
					file: this.resultData,
				});
				console.log(result)
				this.resultData = result.data;
				this.file = result.data;
				xml2js.parseString(result.data, (err, result) => {
					if (err) {
						throw err;
					}
					this.jsData = result;
					const str = JSON.stringify(result, null, 2);
					console.log('js -> %s', str);
				});
			} catch (error) {
				console.error('An error occurred:', error);
				this.errorData = error.response.data
				this.jsData = null
			}
		},
		async addNestedComponent(componentName, parentComponent)
		{
			console.log("Adding component!!!")
			try {
				const result = await axios.post('http://localhost:8000/api/edit', {
					edit_type: "add_child_component",
					parent_component_name: parentComponent,
					child_component_name: componentName,
					file: this.resultData,
				});
				console.log(result)
				this.resultData = result.data;
				this.file = result.data;
				xml2js.parseString(result.data, (err, result) => {
					if (err) {
						throw err;
					}
					this.jsData = result;
					const str = JSON.stringify(result, null, 2);
					console.log('js -> %s', str);
				});
			} catch (error) {
				console.error('An error occurred:', error);
				this.errorData = error.response.data
				this.jsData = null
			}
		},
		async addVariable(componentName, variableName)
		{
			console.log("Adding variable!!!")
			try {
				const result = await axios.post('http://localhost:8000/api/edit', {
					edit_type: "add_variable",
					component_name: componentName,
					variable_name: variableName,
					file: this.resultData,
				});
				console.log(result)
				this.resultData = result.data;
				this.file = result.data;
				xml2js.parseString(result.data, (err, result) => {
					if (err) {
						throw err;
					}
					this.jsData = result;
					const str = JSON.stringify(result, null, 2);
					console.log('js -> %s', str);
				});
			} catch (error) {
				console.error('An error occurred:', error);
				this.errorData = error.response.data
				this.jsData = null
			}
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

.upload-section {
	width: 750px;
	padding: 20px;
}

.edit_button {
  padding: 5px; /* Adjust padding to make button smaller */
  font-size: 12px; /* Adjust font size */
  border-radius: 3px; /* Adjust border radius */
}

.group-wrapper {
	display: flex;
	border-radius: 25px;
	flex-direction: column;
	gap: 10px 20px;
}

.dropzone {
	border: 2px dashed #ccc;
	padding: 20px;
	text-align: center;
	cursor: pointer;
}

.dropzone ul {
	list-style-type: none;
	padding: 0;
}

.dropzone li {
	margin-bottom: 5px;
}

</style>
