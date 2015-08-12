<?php
/**
 * @file
 * Context Datalayer reaction plugin for Context API.
 */

/**
 * Context Reaction plugin to set datalayer values.
 */
class context_datalayer_reaction extends context_reaction {
  /**
   * Implements options_form().
   */
  public function options_form($context) {
    $values = $this->fetch_from_context($context);
    $form = array(
      'class' => array(
        '#title' => t('Section class'),
        '#description' => t('Provides this text as an additional body class (in <strong>$classes</strong> in html.tpl.php) when this section is active.'),
        '#type' => 'textfield',
        '#maxlength' => 64,
        '#default_value' => isset($values['class']) ? $values['class'] : '',
      ),
    );
    return $form;
  }

  /**
   * Set additional classes onto the 'body_classes'.
   */
  public function execute() {
    $classes = array();
    foreach ($this->get_contexts() as $k => $v) {
      if (!empty($v->reactions[$this->plugin]['class'])) {
        datalayer_add(array(
          'dataLayerFoo' => $v->reactions[$this->plugin]['class']
        ));
      }
    }
  }
}
