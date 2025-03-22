def saludar(nombre):
    cpp_callback(cpp_obj, 'Hola desde Python al callback!')
    return f"Hola, {nombre} desde Python!"