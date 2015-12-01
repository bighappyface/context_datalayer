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
    $form = array();

    $form['classes'] = array(
      '#type' => 'container',
      '#prefix' => '<div id="datalayer-settings-ajax-wrapper">',
      '#suffix' => '</div>',
      '#theme' => 'context_datalayer_form',
    );

    $form['classes']['context_datalayer'] = array();
    if (!empty($context->reactions['context_datalayer'])) {
      $form['classes']['remove'] = array();
      foreach (element_children($context->reactions['context_datalayer']) as $class) {
        $form['classes']['context_datalayer'][$class] = array(
          '#type' => 'textfield',
          '#title' => t('Class Name'),
          '#title_display' => 'invisible',
          '#value' => $class,
        );
        $form['classes']['remove'][$class] = array(
          '#type' => 'button',
          '#value' => t('Remove'),
          '#name' => $class,
          '#ajax' => array(
            'callback' => 'context_datalayer_options_form_ajax',
            'wrapper' => 'datalayer-settings-ajax-wrapper',
          ),
          '#limit_validation_errors' => TRUE,
        );
      }
    }

    // Add a new class.
    $form['classes']['add'] = array(
      '#type' => 'container',
    );
    $form['classes']['add']['class'] = array(
      '#type' => 'textfield',
      '#title' => t('Add a section class'),
      '#description' => t('Provides this text as an additional body class (in <strong>$classes</strong> in html.tpl.php) when this section is active.'),
      '#maxlength' => 64,
      '#value' => '',
    );
    $form['classes']['add']['button'] = array(
      '#type' => 'button',
      '#value' => t('Add'),
      '#ajax' => array(
        'callback' => 'context_datalayer_options_form_ajax',
        'wrapper' => 'datalayer-settings-ajax-wrapper',
      ),
      '#limit_validation_errors' => TRUE,
    );

    return $form;
  }

  /**
   * Options form validation handler.
   */
  public function options_form_validate($form, &$form_state) {
    // Context module doesn't allow for validation of reaction fields.
    // @todo: Raise bug for context module.
  }

  /**
   * Options form submit handler.
   */
  public function options_form_submit($values) {
    $classes = array();

    // Build an array of classes.
    if (!empty($values['classes']['context_datalayer'])) {
      foreach ($values['classes']['context_datalayer'] as $class) {
        $classes[$class] = $class;
      }
    }

    // AJAX handler for Add and Remove buttons.
    if (strstr(request_uri(), 'system/ajax')) {
      $form_state = array('submitted' => FALSE);
      $form_build_id = $_POST['form_build_id'];
      $form = form_get_cache($form_build_id, $form_state);

      $form_state['input'] = $_POST;
      $form_state['values'] = array();
      $form = form_builder($form['#form_id'], $form, $form_state);

      switch ($form_state['triggering_element']['#value']) {
        case t('Add'):
          $class = $values['classes']['add']['class'];
          // @todo: Move this to the validation callback.
          if (empty($class)) {
            form_set_error('', t('No class name entered.'));
            break;
          }
          if (in_array($class, $classes)) {
            form_set_error('', t('Class %class already exists.', array('%class' => $class)));
            break;
          }

          $classes[$class] = $class;
          break;

        case t('Remove'):
          unset($classes[$form_state['triggering_element']['#name']]);
          break;
      }
    }

    ksort($classes);
    return $classes;
  }

  /**
   * Execute.
   */
  public function execute() {
    foreach ($this->get_contexts() as $context) {
      if (!empty($context->reactions[$this->plugin])) {
        datalayer_add(array(
          'dataLayerFoo' => array_keys($context->reactions[$this->plugin]),
        ));
      }
    }
  }
}

/**
 * AJAX callback for Context reaction options form.
 */
function context_datalayer_options_form_ajax($form, $form_state) {
  return $form['reactions']['plugins']['context_datalayer']['classes'];
}
