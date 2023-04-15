#include <stdio.h>
#include <math.h>
#include <Python.h>

static double *parse(PyObject *seq)
{
    double *dbar;
    int seqlen;
    int i;

    seq = PySequence_Fast(seq, "argument must be iterable");
    if(!seq)
        return 0;

  
    seqlen = PySequence_Fast_GET_SIZE(seq);
    dbar = malloc(seqlen*sizeof(double)); /* вывделяем блок памяти для списка */

    /* не удалось выделить достаочно памяти */
    if(!dbar) {
        Py_DECREF(seq); /* обнуляем ссылку на объект seq */
        return PyErr_NoMemory();
    }

    for(i=0; i < seqlen; i++) {
        PyObject *fitem;
        PyObject *item = PySequence_Fast_GET_ITEM(seq, i);

        if(!item) {
            Py_DECREF(seq);
            free(dbar);
            return 0;
        }
        fitem = PyNumber_Float(item);
        if(!fitem) {
            Py_DECREF(seq);
            free(dbar);
            PyErr_SetString(PyExc_TypeError, "all items must be numbers");
            return 0;
        }

        dbar[i] = PyFloat_AS_DOUBLE(fitem);
        Py_DECREF(fitem);
    }    

    Py_DECREF(seq);
    
    return dbar;
    
}

static double total(double* data, int len)
{
    double total = 0.0;
    int i;
    for(i=0; i<len; ++i)
        total += data[i];
    return total;
}

static int get_size(PyObject *seq)
{
    seq = PySequence_Fast(seq, "argument must be iterable");
    return PySequence_Fast_GET_SIZE(seq);
}

static int prob_check(double* data, int len)
{
    int i;
    for(i=0; i<len; ++i)
        if (data[i] < 0  || data[i] > 1) return 0;
    return 1;
}

static void check(double* prob, int prob_size, int x_size, int k){

    if (prob_check(prob, prob_size) == 0 || total(prob, prob_size) != 1){
        PyErr_SetString(PyExc_TypeError, "wrong probabilites");
    }

    if (prob_size != x_size){
        PyErr_SetString(PyExc_TypeError, "incompetable dimensions");
    }

    if (k < 0){
        PyErr_SetString(PyExc_TypeError, "invalud value");
    }
}



/* Начальный момент порядка k */
double beg_moment_calc(double * x, double * prob, int n, int k)
{
    int i;
    double res = 0;
    for (i = 0; i < n; i++)
    {
        res = res + pow(x[i], k) * prob[i];
    }
    return res;
}


/* Начальный момент порядка k (Python) */
static PyObject *beg_moment(PyObject *self, PyObject *args)
{
    PyObject* xSeq;
    PyObject* probSeq;
    int k;

    if(!PyArg_ParseTuple(args, "OOi", &xSeq, &probSeq, &k)){
        PyErr_SetString(PyExc_TypeError, "argument must be iterable");
        return 0;
    }

    double *x = parse(xSeq);
    double *prob = parse(probSeq);

    int x_size = get_size(xSeq);
    int prob_size = get_size(probSeq);

    
    check(prob, prob_size, x_size, k);

    double result = beg_moment_calc(x, prob, x_size, k);

    free(x);
    free(prob);

    return Py_BuildValue("d", result);
}

/* Математическое ожидание (Python) */
static PyObject *math_exp(PyObject *self, PyObject *args)
{
    PyObject* xSeq;
    PyObject* probSeq;
    if(!PyArg_ParseTuple(args, "OO", &xSeq, &probSeq)){
        PyErr_SetString(PyExc_TypeError, "argument must be iterable");
        return 0;
    }

    double *x = parse(xSeq);
    double *prob = parse(probSeq);

    int x_size = get_size(xSeq);
    int prob_size = get_size(probSeq);

    
    check(prob, prob_size, x_size, 1);

    double result =beg_moment_calc(x, prob, x_size, 1);

    free(x);
    free(prob);

    return Py_BuildValue("d", result);
}
/* Центральный момент порядка k */
double central_beg_moment_calc(double * x, double * prob, int n, int k)
{
    int i;
    double exp = beg_moment_calc(x, prob, n, 1);
    double res = 0;
    for (i = 0; i < n; i++)
    {
        res = res + pow(x[i] - exp, k) * prob[i];
    }
    return res;
}



/* Центральный момент порядка k  (Python)*/
static PyObject *central_beg_moment(PyObject *self, PyObject *args)
{
    PyObject* xSeq;
    PyObject* probSeq;
    int k;

    if(!PyArg_ParseTuple(args, "OOi", &xSeq, &probSeq, &k)){
        PyErr_SetString(PyExc_TypeError, "argument must be iterable");
        return 0;
    }

    double *x = parse(xSeq);
    double *prob = parse(probSeq);

    int x_size = get_size(xSeq);
    int prob_size = get_size(probSeq);

    
    check(prob, prob_size, x_size, k);

    double result = central_beg_moment_calc(x, prob, x_size, k);

    free(x);
    free(prob);

    return Py_BuildValue("d", result);
}

/* Дисперсия (Python)*/

static PyObject *dispersion(PyObject *self, PyObject *args)
{
    PyObject* xSeq;
    PyObject* probSeq;

    if(!PyArg_ParseTuple(args, "OO", &xSeq, &probSeq)){
        PyErr_SetString(PyExc_TypeError, "argument must be iterable");
        return 0;
    }

    double *x = parse(xSeq);
    double *prob = parse(probSeq);

    int x_size = get_size(xSeq);
    int prob_size = get_size(probSeq);

    
    check(prob, prob_size, x_size, 2);

    double result = central_beg_moment_calc(x, prob, x_size, 2);

    free(x);
    free(prob);

    return Py_BuildValue("d", result);
}

/* Среднее квадратическое отклонение (Python) */
static PyObject *mean_square_deviation(PyObject *self, PyObject *args)
{
    PyObject* xSeq;
    PyObject* probSeq;

    if(!PyArg_ParseTuple(args, "OO", &xSeq, &probSeq)){
        PyErr_SetString(PyExc_TypeError, "argument must be iterable");
        return 0;
    }

    double *x = parse(xSeq);
    double *prob = parse(probSeq);

    int x_size = get_size(xSeq);
    int prob_size = get_size(probSeq);

    
    check(prob, prob_size, x_size, 2);

    double result = pow(central_beg_moment_calc(x, prob, x_size, 2), 0.5);

    free(x);
    free(prob);

    return Py_BuildValue("d", result);
}
/* Ассиметрия Python */
static PyObject *asymmetry(PyObject *self, PyObject *args){

    PyObject* xSeq;
    PyObject* probSeq;

    if(!PyArg_ParseTuple(args, "OO", &xSeq, &probSeq)){
        PyErr_SetString(PyExc_TypeError, "argument must be iterable");
        return 0;
    }

    double *x = parse(xSeq);
    double *prob = parse(probSeq);

    int x_size = get_size(xSeq);
    int prob_size = get_size(probSeq);

    
    check(prob, prob_size, x_size, 1);

    double result = (central_beg_moment_calc(x, prob, x_size, 3) / pow(central_beg_moment_calc(x, prob, x_size, 2), 1.5));
    
    free(x);
    free(prob);

    return Py_BuildValue("d", result);
}
/* Эксцесс (Python)*/

static PyObject *excess(PyObject *self, PyObject *args)
{

    PyObject* xSeq;
    PyObject* probSeq;

    if(!PyArg_ParseTuple(args, "OO", &xSeq, &probSeq)){
        PyErr_SetString(PyExc_TypeError, "argument must be iterable");
        return 0;
    }

    double *x = parse(xSeq);
    double *prob = parse(probSeq);

    int x_size = get_size(xSeq);
    int prob_size = get_size(probSeq);

    
    check(prob, prob_size, x_size, 1);

    double result = central_beg_moment_calc(x, prob, x_size, 4) / pow(central_beg_moment_calc(x, prob, x_size, 2), 2) - 3;
    
    free(x);
    free(prob);

    return Py_BuildValue("d", result);
}

/* Мода */
double mode_calc(double * x, double * prob, int n)
{
    int res = 0;
    int i;
    for (i = 1; i < n; i++)
        if (prob[res] < prob[i])
            res = i;
    return x[res];
}

/* Мода (Python) */
static PyObject *mode(PyObject *self, PyObject *args)
{
    PyObject* xSeq;
    PyObject* probSeq;

    if(!PyArg_ParseTuple(args, "OO", &xSeq, &probSeq)){
        PyErr_SetString(PyExc_TypeError, "argument must be iterable");
        return 0;
    }

    double *x = parse(xSeq);
    double *prob = parse(probSeq);

    int x_size = get_size(xSeq);
    int prob_size = get_size(probSeq);

    
    check(prob, prob_size, x_size, 1);

    double result = mode_calc(x, prob, x_size);

    free(x);
    free(prob);

    return Py_BuildValue("d", result);
}




/* Описывает методы модуля */
static PyMethodDef ownmod_methods[] = {

{   "math_exp",          // название функции внутри python
     math_exp,        // функция C
     METH_VARARGS,   // макрос, поясняющий, что функция у нас с аргументами
     "math expectation" // документация для функции внутри python
},

{   "mode",          // название функции внутри python
     mode,        // функция C
     METH_VARARGS,   // макрос, поясняющий, что функция у нас с аргументами
     "mode" // документация для функции внутри python
},

{   "excess",          // название функции внутри python
     excess,        // функция C
     METH_VARARGS,   // макрос, поясняющий, что функция у нас с аргументами
     "excess" // документация для функции внутри python
},

{   "asymmetry",          // название функции внутри python
     asymmetry,        // функция C
     METH_VARARGS,   // макрос, поясняющий, что функция у нас с аргументами
     "asymmetry" // документация для функции внутри python
},

{   "mean_square_deviation",          // название функции внутри python
     mean_square_deviation,        // функция C
     METH_VARARGS,   // макрос, поясняющий, что функция у нас с аргументами
     "mean_square_deviation" // документация для функции внутри python
},

{   "dispersion",          // название функции внутри python
     dispersion,        // функция C
     METH_VARARGS,   // макрос, поясняющий, что функция у нас с аргументами
     "dispersion" // документация для функции внутри python
},

{   "central_beg_moment",          // название функции внутри python
     central_beg_moment,        // функция C
     METH_VARARGS,   // макрос, поясняющий, что функция у нас с аргументами
     "central_beg_moment" // документация для функции внутри python
},

{   "beg_moment",          // название функции внутри python
     beg_moment,        // функция C
     METH_VARARGS,   // макрос, поясняющий, что функция у нас с аргументами
     "beg_moment" // документация для функции внутри python
},


{ NULL, NULL, 0, NULL } 
// так называемый sentiel. Сколько бы элементов структуры 
// у вас не было, этот нулевой элемент должен быть всегда, и при этом быть последним
};

/* Описываем наш модуль */
static PyModuleDef simple_module = {    
    PyModuleDef_HEAD_INIT,   // обязательный макрос
    "randvar",               // my_plus.__name__
    "amazing documentation", // my_plus.__doc__
    -1,
    ownmod_methods           // сюда передаем методы модуля
};

// в названии функции обязательно должен быть префикс PyInit
PyMODINIT_FUNC PyInit_randvar(void) {
      PyObject* m;
      // создаем модуль
      m = PyModule_Create(&simple_module);
      // если все корректно, то эта проверка не проходит
      if (m == NULL)
          return NULL;
      return m;
}
