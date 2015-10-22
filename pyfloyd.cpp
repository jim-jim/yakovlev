extern "C" {
#include <Python.h>
}
#include <vector>

namespace pyfloyd {	
	typedef std::vector<double>  row_t;
	typedef std::vector<row_t>   matrix_t;

	static matrix_t shortest_dists(const matrix_t &W)
	{
		size_t n = W.size();
		matrix_t D1 = W, D2, tmp;
		D2.resize(n);
		for (size_t i = 0; i < n; i++) D2[i].resize(n);
		for (size_t k = 0; k < n; k++){
			for (size_t i = 0; i < n; i++){
				for (size_t j = 0; j < n; j++){
					double a = D1[i][j], b = D1[i][k] + D1[k][j];
					D2[i][j] = (a < b ? a : b);
				}
			}
			tmp = D2;
			D2 = D1;
			D1 = tmp;
		}
		return D1;
	}
}

static pyfloyd::matrix_t pyobject_to_cxx(PyObject * py_matrix)
{
	pyfloyd::matrix_t result;
	result.resize(PyObject_Length(py_matrix));
	for (size_t i=0; i<result.size(); ++i) {
		PyObject * py_row = PyList_GetItem(py_matrix, i);
		pyfloyd::row_t & row = result[i];
		row.resize(PyObject_Length(py_row));
		for (size_t j=0; j<row.size(); ++j) {
			PyObject * py_elem = PyList_GetItem(py_row, j);
			const double elem = PyFloat_AsDouble(py_elem);
			row[j] = elem;
		}
	}
	return result;
}

static PyObject * cxx_to_pyobject(const pyfloyd::matrix_t &matrix)
{
	PyObject * result = PyList_New(matrix.size());
	for (size_t i=0; i<matrix.size(); ++i) {
		const pyfloyd::row_t & row = matrix[i];
		PyObject * py_row = PyList_New(row.size());
		PyList_SetItem(result, i, py_row);
		for (size_t j=0; j<row.size(); ++j) {
			const double elem = row[j];
			PyObject * py_elem = PyFloat_FromDouble(elem);
			PyList_SetItem(py_row, j, py_elem);
		}
	}
	return result;
}

static PyObject * pyfloyd_shortest_dists(PyObject * module, PyObject * pyW)
{
	const pyfloyd::matrix_t W = pyobject_to_cxx(pyW);
	const pyfloyd::matrix_t D = pyfloyd::shortest_dists(W);
	PyObject * pyD = cxx_to_pyobject(D);
	return pyD;
}

PyMODINIT_FUNC PyInit_pyfloyd()
{
	static PyMethodDef ModuleMethods[] = {
		{ "shortest_dists", pyfloyd_shortest_dists, METH_O, "All pairwise distances" },
		{ NULL, NULL, 0, NULL }
	};
	static PyModuleDef ModuleDef = {
		PyModuleDef_HEAD_INIT,
		"pyfloyd",
		"Floyd-Warshall algorithms",
		-1, ModuleMethods, 
		NULL, NULL, NULL, NULL
	};
	PyObject * module = PyModule_Create(&ModuleDef);
	return module;
}
