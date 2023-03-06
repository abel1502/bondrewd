# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'Bondrewd'
copyright = '2023, abel1502'
author = 'abel1502'

release = '0.0'
version = '0.0.0dev1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'myst_parser',
    'sphinx.ext.duration',
    'sphinx.ext.autosummary',
    'sphinx.ext.intersphinx',
]

myst_enable_extensions = [
    'colon_fence',
    'attrs_inline',
    'attrs_block',
    'strikethrough',
    'replacements',
    # 'smartquotes',
    'tasklist',
    'deflist',
    'fieldlist',
]

intersphinx_mapping = {
    'python': ('https://docs.python.org/3/', None),
    'sphinx': ('https://www.sphinx-doc.org/en/master/', None),
}
intersphinx_disabled_domains = ['std']

templates_path = ['_templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
# html_static_path = ['_static']

# -- Options for EPUB output
epub_show_urls = 'footnote'
