from distutils.core import setup, Extension

module1 = Extension(
	'randvar', # название модуля внутри Python
	 sources = ['test.c'] # Исходные файлы модуля
)

setup(
	name = 'randvar',              # название модуля (my_plus.__name__)
	version = '1.0',               # версия модуля
	description = 'Simple module', # описание модуля
	ext_modules= [module1]         # объекты типа Extension (мы объявили его выше)
)

