<template>
	<div v-html="equation"></div>
  </template>
  
  <script>
  import axios from 'axios';

  export default {
	props: {
	  propEquation: String,
	},
	data() {
	  return {
		equation: '',
		placeHolder: `
		<math xmlns="http://www.w3.org/1998/Math/MathML">
		<mrow>
			<mi>x</mi>
			<mo>=</mo>
			<mfrac>
			<mrow>
				<mo form="prefix">−<!-- minus sign --></mo>
				<mi>b</mi>
				<mo>±<!-- ± --></mo>
				<msqrt>
				<msup>
					<mi>b</mi>
					<mn>2</mn>
				</msup>
				<mo>−<!-- minus sign --></mo>
				<mn>4</mn>
				<mo>⁢<!-- ⁢ --></mo>
				<mi>a</mi>
				<mo>⁢<!-- ⁢ --></mo>
				<mi>c</mi>
				</msqrt>
			</mrow>
			<mrow>
				<mn>2</mn>
				<mo>⁢<!-- ⁢ --></mo>
				<mi>a</mi>
			</mrow>
			</mfrac>
		</mrow>
		</math>`
	  };
	},
	watch: {
		propEquation: {
			immediate: true,
			async handler(newVal) {
				if (newVal) {
					const cleanedEquation = this.replaceTags(newVal);
					console.log('before converting', cleanedEquation);
					const convertedEquation = await this.convertContentToPresentationMathML(cleanedEquation);
					console.log('after converting', convertedEquation);
					if (convertedEquation) {
						this.equation = convertedEquation;
					}
					else {
						this.equation = this.placeHolder;
					}
				} else {
					this.equation = this.placeHolder;
				}
			}
		}
	},
	methods: {
		async convertContentToPresentationMathML(contentMathML) {
			try {
				const response = await axios.get(`https://www.wiris.net/demo/editor/content2mathml?mml=${encodeURIComponent(contentMathML)}`);
				return response.data;
			} catch (error) {
				console.error('Error converting MathML:', error);
				return null;
			}
		},
		replaceTags(equation) {
			console.log("HERE")
			equation = equation.replace(/<apply id="[^"]+">/g, '<apply>');
			equation = equation.replace(/<cn cellml:units="[^"]+">/g, '<cn>');
			return equation
		},
	},
  };
  </script>